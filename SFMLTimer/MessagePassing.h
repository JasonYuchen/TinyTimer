/**************************************************************
*  Filename:    MessagePassing.h
*  
*  <C++ Concurrency In Action> appendix C
*  Anthony Williams
**************************************************************/

#pragma once
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

namespace messaging
{
	struct message_base                                           // Base class of queue entries
	{
		virtual ~message_base() {}
	};

	template<typename Msg>                                        // Each message type has a specialization
	struct wrapped_message : message_base
	{
		Msg contents;
		explicit wrapped_message(const Msg &contents_) :contents(contents_) {}
	};
	                                
	class queue                                                   // Message queue
	{
	private:
		std::mutex m;
		std::condition_variable c;
		std::queue<std::shared_ptr<message_base>> q;              // Actual queue stores pointers to message_base
	public:
		template<typename T>
		void push(const T &msg)
		{
			std::lock_guard<std::mutex> lk(m);
			q.push(std::make_shared<wrapped_message<T>>(msg));    // Wrap message and store pointer
			c.notify_all();
		}
		std::shared_ptr<message_base> wait_and_pop()
		{
			std::unique_lock<std::mutex> lk(m);
			c.wait(lk, [&]() {return !q.empty(); });              // Block until queue isn't empty
			auto res = q.front();
			q.pop();
			return res;
		}
	};

	class sender
	{
	private:
		queue *q;                                                 // Sender is wrapper around queue pointer
	public:
		sender() :q(nullptr) {}                                   // Default-constructed sender has no queue
		explicit sender(queue *q_) :q(q_) {}
		template<typename Message>
		void send(const Message &msg)                             // Sending pushes message on the queue
		{
			if (q)
			{
				q->push(msg);
			}
		}
	};

	class receiver
	{
	private:
		queue q;                                                  // A receiver owns the queue
	public:
		operator sender()                                         // Implicit conversion to a sender
		{
			return sender(&q);
		}
		dispatcher wait()                                         // Waiting for a queue creates a dispatcher
		{
			return dispatcher(&q);
		}
	};

	class close_queue {};                                         // For closing the queue

	class dispatcher
	{
	private:
		queue *q;
		bool chained;
		dispatcher(const dispatcher &) = delete;
		dispatcher &operator=(const dispatcher &) = delete;

		template<typename Dispatcher, typename Msg. typename Func>
		friend class TemplateDispatcher;                          // Allow TemplateDispatcher instances to access the internals

		void wait_and_dispatch()
		{
			for (;;)                                              // Loop, waiting for and dispatching messages
			{
				auto msg = q->wait_and_pop();
				dispatch(msg);
			}
		}
		bool dispatch(const std::shared_ptr<message_base> &msg)
		{
			if (dynamic_cast<wrapped_message<close_queue>*>(msg.get()))
			{
				throw close_queue();
			}
			return false;                                         // false indicate the message was unhandled
		}
	public:
		dispatcher(dispatcher &&other) : q(other.q), chained(other.chained)
		{
			other.chained = true;
		}
		explicit dispatcher(queue *q_) :q(q_), chained(false) {}

		template<typename Message, typename Func>
		TemplateDispatcher<dispatcher, Message, Func>
		handle(Func &&f)                                          // Handle a specific type of message with a TemplateDispatcher
		{
			return TemplateDispatcher<dispatcher, Message, Func>(q, this, std::forward<Func>(f));
		}

		~dispatcher() noexcept(false)                             // may throw close_queue exception
		{
			if (!chained)
			{
				wait_and_dispatch();
			}
		}
	};

	template<typename PreviousDispatcher, typename Msg, typename Func>
	class TemplateDispatcher
	{
	private:
		queue *q;
		PreviousDispatcher *prev;
		Func f;
		bool chained;
		TemplateDispatcher(const TemplateDispatcher &) = delete;
		TemplateDispatcher &operator=(const TemplateDispatcher &) = delete;

		template<typename Dispatcher, typename OtherMsg, typename OtherFunc>
		friend class TemplateDispatcher;                          // TemplateDispatcher instantiations are friends of each other

		void wait_and_dispatch()
		{
			for (;;)
			{
				auto msg = q->wait_and_pop();
				if (dispatch(msg))                                // If we handle the message, break out of the loop
					break;
			}
		}
		bool dispatch(const std::shared_ptr<message_base> &msg)
		{
			if (wrapped_message<Msg> *wrapper = dynamic_cast<wrapped_message<Msg>*>(msg.get()))
			{
				f(wrapper->content);                              // Check the message type and call the function
				return true;
			}
			else
			{
				return prev->dispatch(msg);                       // Chain to the previous dispatcher
			}
		}
	public:
		TemplateDispatcher(TemplateDispatcher &&other) :q(other.q), prev(other.prev), f(std::move(other.f)), chained(other.chained)
		{
			other.chained = true;
		}
		TemplateDispatcher(queue *q_, PreviousDispatcher *prev_, Func &&f_) :q(q_), prev(prev_), f(std::forward<Func>(f_)), chained(false)
		{
			prev_->chained = true;
		}

		template<typename OtherMsg, typename OtherFunc>
		TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>
		handle(OtherFunc &&of)                                    // Additional handlers can be chained
		{
			return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>(q, this, std::forward<OtherFunc>(of));
		}

		~TemplateDispatcher() noexcept(false)
		{
			if (!chained)
			{
				wait_and_dispatch();
			}
		}
	};
}

class atm
{
	messaging::receiver incoming;
	messaging::sender bank;
	messaging::sender interface_hardware;
	void (atm::*state)();
	std::string account;
	unsigned withdrawal_amount;
	std::string pin;
	void process_withdrawal()
	{
		incoming.wait()
			.handle<withdraw_ok>(
				[&](withdraw_ok const& msg)
		{
			interface_hardware.send(
				issue_money(withdrawal_amount));
			bank.send(
				withdrawal_processed(account, withdrawal_amount));
			state = &atm::done_processing;
		}
				)
			.handle<withdraw_denied>(
				[&](withdraw_denied const& msg)
		{
			interface_hardware.send(display_insufficient_funds());
			state = &atm::done_processing;
		}
				)
			.handle<cancel_pressed>(
				[&](cancel_pressed const& msg)
		{
			bank.send(
				cancel_withdrawal(account, withdrawal_amount));
			interface_hardware.send(
				display_withdrawal_cancelled());
			state = &atm::done_processing;
		}
		);
	}
...
#pragma once
#include<string>
#include<Windows.h>

/*Get current program's name
 *exception handler required*/
std::wstring getProgName();

/*Get current focused window's name
 *exception handler required*/
std::wstring getWindowTitle();

/*Set console cursor postion
 0--------------------------> x
 |
 |
 |
 |
 y
 *exception handler required*/
void SetAbsLocate(int x, int y);

/*Compare 2 SYSTEMTIME classes based on GetDiffSeconds,
 *if lhs is earlier than rhs in timeline, then return true*/
bool operator< (const SYSTEMTIME &lhs, const SYSTEMTIME &rhs);

/*output a SYSTEMTIME in the format as follows
 *hh:mm:ss(e.g. 19:56:04)*/
std::wostream &operator<<(std::wostream &os, const SYSTEMTIME &rhs);

/*return difference based on seconds between 2 SYSTEMTIME classes
 *e.g. 19:56:04 - 19:56:00 = 4*/
int GetDiffSeconds(const SYSTEMTIME &t1, const SYSTEMTIME &t2);
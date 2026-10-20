/*
 * log.h
 *
 *  Created on: Apr 12, 2026
 *      Author: bitstream
 */

#ifndef INC_LOG_H_
#define INC_LOG_H_

#include <stdarg.h>
#ifdef __cplusplus
class Log
{
public:
	static void Init();

	static void Pause();
	static void Resume();

	static void I(const char *format, ...);
	static void W(const char *format, ...);
	static void E(const char *format, ...);
	static void write(const char * tag, const char *format, va_list ap);
private:

};
#endif

#ifdef __cplusplus
extern "C" {
#endif

void LogInit(void);
void LogI(const char *format, ...);
void LogW(const char *format, ...);
void LogE(const char *format, ...);

void LogPause();
void LogResume();

#ifdef __cplusplus
}
#endif

#endif /* INC_LOG_H_ */

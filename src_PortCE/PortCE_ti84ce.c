/**
 * @file
 *
 * @author "zerico2005"
 */

/*
**	PortCE_ti84ce.h simulates libraries and functions from the CE Programming Toolchain.
**	Some functions may not be completely accurate or fully implemented.
*/

#include "PortCE_Common.h"

#include "PortCE_include/ce/include/sys/lcd.h"
#include "PortCE_include/ce/include/sys/util.h"
#include "PortCE_include/ce/include/sys/timers.h"

// #include <process.h>
//#include <dir.h>
// #include <conio.h>
// #include <sys/stat.h>
// #include <sys/types.h>
// #include <unistd.h>

extern uint8_t simulated_ram[16777216];

/* <sys/util.h> */

uint32_t ti_random(void) {
	return (uint32_t)rand();
}

void ti_srandom(uint32_t seed) {
	srand(seed);
}

/** Not actually atomic */
uint32_t atomic_load_32(volatile uint32_t* p) {
	return *p;
}

/** Not actually atomic */
uint32_t atomic_load_increasing_32(volatile uint32_t* p) {
	uint32_t read_0 = *p;
	uint32_t read_1 = *p;
	return MAX(read_0, read_1);
}

/** Not actually atomic */
uint32_t atomic_load_decreasing_32(volatile uint32_t* p) {
	uint32_t read_0 = *p;
	uint32_t read_1 = *p;
	return MIN(read_0, read_1);
}


/* Debug */

	void boot_DebugPrintf(const char *string) {
		printf("%s",string);
	}

/* <sys/rtc.h> */

	static uint8_t  calc_day     = 1   ;
	static uint8_t  calc_month   = 1   ;
	static uint16_t calc_year    = 2015;
	static uint8_t  calc_hours   = 0   ;
	static uint8_t  calc_minutes = 0   ;
	static uint8_t  calc_seconds = 0   ;

	/** @todo Add proper date validation */
	void boot_SetDate(uint8_t day, uint8_t month, uint16_t year) {
		if (day > 31 || month > 12 || year < 2015) {
			return;
		}
		calc_year  = year ;
		calc_month = month;
		calc_day   = day  ;
	}

	void boot_GetDate(uint8_t *day, uint8_t *month, uint16_t *year) {
		if (day   != NULL) { *day   = calc_day  ; }
		if (month != NULL) { *month = calc_month; }
		if (year  != NULL) { *year  = calc_year ; }
	}

	void boot_SetTime(uint8_t seconds, uint8_t minutes, uint8_t hours) {
		if (seconds >= 60 || minutes >= 60 || hours >= 24) {
			return;
		}
		calc_seconds = seconds;
		calc_minutes = minutes;
		calc_hours   = hours  ;
	}

	void boot_GetTime(uint8_t *seconds, uint8_t *minutes, uint8_t *hours) {
		if (seconds != NULL) { *seconds = calc_seconds; }
		if (minutes != NULL) { *minutes = calc_minutes; }
		if (hours   != NULL) { *hours   = calc_hours  ; }
	}

	bool boot_IsAfterNoon(void) {
		return (calc_hours >= 12) ? true : false;
	}

/* <sys/timers.h> */

	// struct Simulated_Timer {
	// 	uint8_t enabled;
	// 	uint8_t rate;
	// 	uint8_t inter;
	// 	uint8_t dir;
	// }; typedef struct Simulated_Timer Simulated_Timer;
	
	// static Simulated_Timer simulated_timer[3] = {
	// 	{0,1,0,1},{1,1,0,1},{1,1,0,1}
	// };

	// void timer_Enable(uint8_t n, uint8_t rate, uint8_t inter, uint8_t dir) {
	// 	if (n < 1 || n > 3) { return; }
	// 	simulated_timer[n - 1].enabled = 1;
	// 	simulated_timer[n - 1].rate = rate;
	// 	simulated_timer[n - 1].inter = inter;
	// 	simulated_timer[n - 1].dir = dir;
	// }
	
	// void timer_Disable(uint8_t n) {
	// 	if (n < 1 || n > 3) { return; }
	// 	simulated_timer[n - 1].enabled = 0;
	// }

	// // n is supposed to be a value between 1 and 3
	// uint32_t timer_Get(uint8_t n) {
	// 	if (n < 1 || n > 3) { return 0; }
	// 	if (simulated_timer[n - 1].enabled == 0) {
	// 		return 0;
	// 	}
	// 	if (simulated_timer[n - 1].rate == 0) {
	// 		fp64 timerCPU = getDecimalTime(); //Time in seconds
	// 		timerCPU *= 15000000.0;
	// 		return (uint32_t)timerCPU;
	// 	}
	// 	fp64 timer32K = getDecimalTime(); //Time in seconds
	// 	timer32K *= 32768.0;
	// 	return (uint32_t)timer32K;
	// }

	/* Sleep and Delays */

		void delay(uint16_t msec) {
			nano64_t dur = (nano64_t)msec * 1000000;
			nano64_t startTime = getNanoTime();
			PortCE_new_frame();
			while (getNanoTime() - startTime < dur) {
				/* Wait */
			}
		}
		
		void msleep(uint16_t msec) {
			nano64_t dur = (nano64_t)msec * 1000000;
			nano64_t startTime = getNanoTime();
			PortCE_new_frame();
			while (getNanoTime() - startTime < dur) {
				/* Wait */
			}
		}

		ti_unsigned_int sleep(ti_unsigned_int seconds) {
			nano64_t dur = (nano64_t)seconds * 1000000000;
			nano64_t startTime = getNanoTime();
			while (getNanoTime() - startTime < dur) {
				PortCE_new_frame();
			}
			return 0;
		}

		void ticksleep(ti_unsigned_long ticks) {
			nano64_t dur = (nano64_t)((fp64)ticks * (1000000000.0 / (fp64)TI_CLOCKS_PER_SEC));
			nano64_t startTime = getNanoTime();
			PortCE_new_frame();
			while (getNanoTime() - startTime < dur) {
				/* Wait */
			}
		}

		ti_int usleep(useconds_t usec) {
			nano64_t dur = (nano64_t)usec * 1000;
			nano64_t startTime = getNanoTime();
			PortCE_new_frame();
			while (getNanoTime() - startTime < dur) {
				/* Wait */
			}
			return 0;
		}

		void boot_WaitShort(void) {
			delay(10);
		}

/* <fileioc.h> */
	
	// #define NameSize (8)
	// #define RamFolderPath "./Ti-Ram"
	// #define ArchiveFolderPath "./Ti-Archive"

	// __attribute__((unused)) static const uint8_t File_8XV_Text[] = {
	// /* X      0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |  A |  B |  C |  D |  E |  F */
    // /* 0 */  '*', '*', 'T', 'I', '8', '3', '*',0x1A,0x0A,'\0','\0','\0','\0','\0','\0','\0',
    // /* 1 */ '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
    // /* 2 */ '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
    // /* 3 */ '\0','\0','\0','\0','\0'
	// };

	//  typedef struct __attribute__((packed)) Ti_File_Structure  {
	// 	char header[53];
	// 	uint16_t file_size; /** 17 bytes more than data_size */
	// 	uint8_t value_0x0D;
	// 	uint8_t valueA_0x00;
	// 	uint16_t data_size; /** size of actual data */
	// 	uint8_t value_0x15;
	// 	char name[8];
	// 	uint8_t valueB_0x00;
	// 	uint8_t value_0x80;
	// 	unsigned char data[]; /** size = data_size */
	// } Ti_File_Structure;
	
	// typedef struct Ti_File_Handle {
	// 	bool active;
	// 	bool read;
	// 	bool write;
	// 	bool append;
	// 	FILE* file;
	// 	unsigned char* data;
	// 	char name[NameSize + 1];
	// } Ti_File_Handle;
	
	// static Ti_File_Handle ti_File_Handle[256];

	// bool validate_Ti_File_Handle(uint8_t handle) {
	// 	if (handle == 0) {
	// 		return false;
	// 	}
	// 	if (ti_File_Handle[handle].active == false) {
	// 		return false;
	// 	}
	// 	if (ti_File_Handle[handle].file == NULL) {
	// 		return false;
	// 	}
	// 	if (
	// 		(ti_File_Handle[handle].read == false) &&
	// 		(ti_File_Handle[handle].write == false) &&
	// 		(ti_File_Handle[handle].append == false)
	// 	) {
	// 		return false;
	// 	}
	// 	return true;
	// }

	// void clear_Ti_File_Handle(uint8_t handle) {
	// 	ti_File_Handle[handle].active = false;
	// 	ti_File_Handle[handle].read = false;
	// 	ti_File_Handle[handle].write = false;
	// 	ti_File_Handle[handle].append = false;
	// 	memset(ti_File_Handle[handle].name,'\0',sizeof(ti_File_Handle[handle].name));
	// }

	// uint8_t ti_Open(const char* name, const char* mode) {
	// 	if (name == NULL || mode == NULL) {
	// 		return 0;
	// 	}
	// 	uint8_t handle = 0;
	// 	for (size_t i = 1; i < 256; i++) {
	// 		if (ti_File_Handle[i].active == false) {
	// 			handle = i;
	// 			break;
	// 		}
	// 	}
	// 	if (handle == 0) {
	// 		return 0;
	// 	}
	// 	clear_Ti_File_Handle(handle);
	// 	switch (mode[0]) {
	// 		case 'r':
	// 			ti_File_Handle[handle].read = true;
	// 		break;
	// 		case 'w':
	// 			ti_File_Handle[handle].write = true;
	// 		break;
	// 		case 'a':
	// 			ti_File_Handle[handle].append = true;
	// 		break;
	// 		default:
	// 			return 0;
	// 	};
	// 	if (mode[1] == '+') {
	// 		if (ti_File_Handle[handle].read == true) {
	// 			ti_File_Handle[handle].write = true;
	// 		} else {
	// 			ti_File_Handle[handle].read = true;
	// 		}
	// 	}

	// 	snprintf(ti_File_Handle[handle].name,NameSize + 1,"%s",name);
	// 	for (size_t i = 0; i < NameSize; i++) { // Capitalizes
	// 		if (ti_File_Handle[handle].name[i] >= 'a' && ti_File_Handle[handle].name[i] <= 'z') {
	// 			ti_File_Handle[handle].name[i] -= ('a' - 'A');
	// 		}
	// 	}

	// 	size_t filePathSize = snprintf(NULL,0,"%s/%s.dat",RamFolderPath,ti_File_Handle[handle].name);
	// 	filePathSize++; // Null Terminator
	// 	char* filePath = (char*)malloc(filePathSize + 1);
	// 	snprintf(filePath,filePathSize,"%s/%s.dat",RamFolderPath,ti_File_Handle[handle].name);
		
	// 	size_t fileModeSize = snprintf(NULL,0,"%sb",mode);
	// 	fileModeSize++; // Null Terminator
	// 	char* fileMode = (char*)malloc(fileModeSize + 1);
	// 	snprintf(fileMode,fileModeSize,"%sb",mode);
		
	// 	mkdir(RamFolderPath);
	// 	ti_File_Handle[handle].file = fopen(filePath, fileMode);
		
	// 	if (ti_File_Handle[handle].file == NULL) {
	// 		printf_FileIOC("\nError: ti_Read() could not open: %s",filePath);
	// 		FREE(fileMode);
	// 		FREE(filePath);
	// 		return 0;
	// 	}
	// 	ti_File_Handle[handle].active = true;
	// 	FREE(fileMode);
	// 	FREE(filePath);
	// 	return handle;
	// }

	// int ti_Close(uint8_t handle) { 
	// 	fclose(ti_File_Handle[handle].file);
	// 	clear_Ti_File_Handle(handle);
	// 	return handle;
	// }

	// size_t ti_Read(void* data, size_t size, size_t count, uint8_t handle) {
	// 	if (data == NULL || size == 0 || count == 0)	{
	// 		printf_FileIOC("\nError: ti_Read() NULL parameters");
	// 		return 0;
	// 	}
	// 	if (validate_Ti_File_Handle(handle) == false) {
	// 		printf_FileIOC("\nError: ti_Read() handle %d is invalid or NULL",handle);
	// 		return 0;
	// 	}
	// 	if (ti_File_Handle[handle].read == false) {
	// 		printf_FileIOC("\nError: ti_Read() handle %d does not have Read premissions",handle);
	// 		return 0;
	// 	}
	// 	size_t readCount = fread(data, size, count, ti_File_Handle[handle].file);
	// 	return readCount;
	// }
	
	// size_t ti_Write(const void* data, size_t size, size_t count, uint8_t handle) {
	// 	if (data == NULL || size == 0 || count == 0)	{
	// 		printf_FileIOC("\nError: ti_Write() NULL parameters");
	// 		return 0;
	// 	}
	// 	if (validate_Ti_File_Handle(handle) == false) {
	// 		printf_FileIOC("\nError: ti_Write() handle %d is invalid or NULL",handle);
	// 		return 0;
	// 	}
	// 	if (ti_File_Handle[handle].write == false && ti_File_Handle[handle].append == false) {
	// 		printf_FileIOC("\nError: ti_Write() handle %d does not have Write/Append premissions",handle);
	// 		return 0;
	// 	}
	// 	size_t writeCount = fwrite(data,size,count,ti_File_Handle[handle].file);
	// 	return writeCount;
	// }

	// int24_t ti_PutC(char ch, uint8_t handle) {
	// 	if (validate_Ti_File_Handle(handle) == false) {
	// 		printf_FileIOC("\nError: ti_PutC() handle %d is invalid or NULL",handle);
	// 		return EOF;
	// 	}
	// 	return putc(ch,ti_File_Handle[handle].file);
	// }
	// int24_t ti_GetC(uint8_t handle) {
	// 	if (validate_Ti_File_Handle(handle) == false) {
	// 		printf_FileIOC("\nError: ti_GetC() handle %d is invalid or NULL",handle);
	// 		return EOF;
	// 	}
	// 	return getc(ti_File_Handle[handle].file);
	// }
	// int24_t ti_Seek(int24_t offset, uint24_t origin, uint8_t handle) {
	// 	if (validate_Ti_File_Handle(handle) == false) {
	// 		printf_FileIOC("\nError: ti_Seek() handle %d is invalid or NULL",handle);
	// 		return EOF;
	// 	}
	// 	return fseek(ti_File_Handle[handle].file,offset,origin);
	// }

	// uint16_t ti_GetSize(uint8_t handle) {
	// 	if (validate_Ti_File_Handle(handle) == false) {
	// 		printf_FileIOC("\nError: ti_GetSize() handle %d is invalid or NULL",handle);
	// 		return 0;
	// 	}
	// 	fpos_t currentPos;
	// 	fgetpos(ti_File_Handle[handle].file,&currentPos);
	// 	fseek(ti_File_Handle[handle].file, 0, SEEK_END);
	// 	long fileSize = ftell(ti_File_Handle[handle].file);
	// 	fsetpos(ti_File_Handle[handle].file,&currentPos);
	// 	valueLimit(fileSize,0,65535);
	// 	return (uint16_t)fileSize;
	// }

	// int24_t ti_Rewind(uint8_t handle) {
	// 	return ti_Seek(0, SEEK_SET, handle);
	// }
	
	// void* ti_GetDataPtr(__attribute__((unused)) uint8_t handle) {
	// 	return NULL;
	// }

	// #undef RamFolderPath
	// #undef NameSize

/* Update */

// typedef struct Ti_Timer {
// 	uint32_t Counter;
// 	uint32_t ReloadValue;
// 	uint32_t MatchValue_1;
// 	uint32_t MatchValue_2;
// } Ti_Timer;

// static Ti_Timer* const timer_list = (Ti_Timer*)((void*)&simulated_ram[0xF20000]);
// static const size_t timer_count = 3;

static const fp64 timer_mult = 1.0;

ti_clock_t ti_clock() {
	return (ti_clock_t)((fp64)clock() * ((fp64)TI_CLOCKS_PER_SEC / (fp64)CLOCKS_PER_SEC) * timer_mult);
}

static nano64_t last_timer_update = 0;

static void PortCE_update_timers(void) {
	if (last_timer_update == 0) {
		last_timer_update = getNanoTime();
	}
	const nano64_t current_time = getNanoTime();
	
	const nano64_t delta_nano = (current_time - last_timer_update);
	const uint32_t delta_32K = (uint32_t)((int32_t)((fp64)delta_nano * (32768.0 / 1.0e9) * timer_mult)); // 32 KHz
	const uint32_t delta_CPU = (uint32_t)((int32_t)((fp64)delta_nano * (8.0e6 / 1.0e9) * timer_mult)); // 8 MHz
	if (delta_32K == 0 || delta_CPU == 0) {
		return; // Prevents infinite loops if not enough time passes between updates 
	}
	/* Update timers */
		if (timer_Control & TIMER1_ENABLE) {
			if (timer_Control & TIMER1_UP) {
				timer_1_Counter += (timer_Control & TIMER1_32K) ? delta_32K : delta_CPU;
			} else {
				timer_1_Counter -= (timer_Control & TIMER1_32K) ? delta_32K : delta_CPU;
			}
		}
		if (timer_Control & TIMER2_ENABLE) {
			if (timer_Control & TIMER2_UP) {
				timer_2_Counter += (timer_Control & TIMER2_32K) ? delta_32K : delta_CPU;
			} else {
				timer_2_Counter -= (timer_Control & TIMER2_32K) ? delta_32K : delta_CPU;
			}
		}
		if (timer_Control & TIMER3_ENABLE) {
			if (timer_Control & TIMER3_UP) {
				timer_3_Counter += (timer_Control & TIMER3_32K) ? delta_32K : delta_CPU;
			} else {
				timer_3_Counter -= (timer_Control & TIMER3_32K) ? delta_32K : delta_CPU;
			}
		}

	last_timer_update = current_time;
}


ti_int PortCE_update_registers(void) {
	PortCE_update_timers();
	return 0;
}

/* Setup */

void reset_ti84ce_registers(void) {
	lcd_Timing0 = 0x1F0A0338;
	lcd_Timing1 = 0x0402093F;
	lcd_Timing2 = 0x00EF7802;
	lcd_Timing3 = 0x00000000;

	lcd_UpBaseFull = (uint32_t)RAM_OFFSET(lcd_Ram);
	lcd_LpBaseFull = 0x00000000;
	lcd_Control    = LCD_BGR16bit;

	lcd_EnableInt       = 0;
	lcd_IntStatus       = 0;
	lcd_IntStatusMasked = 0;
	lcd_IntAcknowledge  = 0;
	lcd_UpBaseCurrFull  = 0;
	lcd_LpBaseCurrFull  = 0;

	lcd_CrsrCtrl            = 0;
	lcd_CrsrConfig          = 0;
	lcd_CrsrPalette0        = 0;
	lcd_CrsrPalette1        = 0;
	lcd_CrsrXY              = 0;
	lcd_CrsrXYFull          = 0;
	lcd_CrsrX               = 0;
	lcd_CrsrY               = 0;
	lcd_CrsrYFull           = 0;
	lcd_CrsrClip            = 0;
	lcd_CrsrClipX           = 0;
	lcd_CrsrClipY           = 0;
	lcd_CrsrEnableInt       = 0;
	lcd_CrsrIntAcknowledge  = 0;
	lcd_CrsrIntStatus       = 0;
	lcd_CrsrIntStatusMasked = 0;

	lcd_BacklightLevel = 0x80; // Unsure what the default is

	calc_day     = 1   ;
	calc_month   = 1   ;
	calc_year    = 2015;
	calc_hours   = 0   ;
	calc_minutes = 0   ;
	calc_seconds = 0   ;

	/* Timers */
		memset(&simulated_ram[0xF20000], 0, 0x38);
		last_timer_update = getNanoTime();
}
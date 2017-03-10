//
//
//

#include "SD_ListFiles.h"

#include "genieArduino.h"
#include "Touch_Screen_Definitions.h"
#include "Marlin.h"
#include "Configuration.h"
#include "stepper.h"
#include "temperature.h"
#include "cardreader.h"


Listfiles::Listfiles(){
	dias=-1, horas=-1, minutos=-1;
	filmetros1 = 0 ,filmetros2=0;
	filgramos1 = 0 ,filgramos2=0;
	commandline[50];
	commandline2[18];
	simplify3D = -1;
}


void Listfiles::get_lineduration(void){
	
	card.openFile(card.filename, true);
	dias=-1, horas=-1, minutos=-1;
	char serial_char='\0';
	int posi = 0;
	int linecomepoint=0;
	simplify3D=-1;
	int exit = 0;
	while(linecomepoint < 5 && !card.isEndFile()){
		memset(commandline, '\0', sizeof(commandline) );
		while(commandline[0]!=';' && !card.isEndFile()){
			serial_char='\0';
			posi = 0;
			while(serial_char != '\n' && posi < 49 && !card.isEndFile()){
				
				int16_t n=card.get();
				serial_char = (char)n;
				commandline[posi]=serial_char;
				
				posi++;
			}
			exit++;
			if(exit>20 || card.isEndFile()){
				linecomepoint = 5;
				dias=0;
				horas=0;
				minutos = 1;
				filgramos1 = 0;
				filgramos2 = 0;
				card.closefile();
				return;
			}
		}
		if(linecomepoint == 0){
			if (extract_data_Symplify() !=-1 ){
				linecomepoint = 5;
				simplify3D = 1;
			}
			
			else{
				simplify3D = 0;
			}
		}
		else{
			extract_data();
			if (minutos !=-1 ){
				linecomepoint = 5;
			}
		}
		
		linecomepoint++;
	}
	//Serial.println(simplify3D);
	if(simplify3D == 0){
		
		memset(commandline, '\0', sizeof(commandline) );
		posi = 0;
		serial_char='\0';
		
		while(serial_char != '\n' && posi < 49 && !card.isEndFile()){
			
			int16_t n=card.get();
			serial_char = (char)n;
			commandline[posi]=serial_char;
			posi++;
		}
		extract_data1();
		//card.closefile();
		if(minutos == -1){
			dias=0;
			horas=0;
			minutos = 1;
			filgramos1 = 0;
			filgramos2 = 0;
		}
		memset(commandline, '\0', sizeof(commandline) );
	}
	else if (simplify3D == 1){

		card.sdfinalline();
		memset(commandline, '\0', sizeof(commandline) );
		
		linecomepoint=0;
		exit = 0;
		while(linecomepoint < 2 && !card.isEndFile()){
			memset(commandline, '\0', sizeof(commandline) );
			while(commandline[0]!=';' && !card.isEndFile()){
				serial_char='\0';
				posi = 0;
				while(serial_char != '\n' && posi < 49 && !card.isEndFile()){
					
					int16_t n=card.get();
					serial_char = (char)n;
					commandline[posi]=serial_char;
					
					posi++;
				}
				exit++;
				if(exit>20 || card.isEndFile()){
					linecomepoint = 5;
					dias=0;
					horas=0;
					minutos = 1;
					filgramos1 = 0;
					filgramos2 = 0;
					card.closefile();
					return;
				}
				
			}
			
			linecomepoint++;
		}
		extract_data();
		memset(commandline, '\0', sizeof(commandline) );
		posi = 0;
		serial_char='\0';
		linecomepoint = 0;
		exit = 0;
		while(linecomepoint < 3 && !card.isEndFile()){
			memset(commandline, '\0', sizeof(commandline) );
			while(commandline[0]!=';' && !card.isEndFile()){
				serial_char='\0';
				posi = 0;
				while(serial_char != '\n' && posi < 49 && !card.isEndFile()){
					
					int16_t n=card.get();
					serial_char = (char)n;
					commandline[posi]=serial_char;
					
					posi++;
				}
				exit++;
				if(exit>20 || card.isEndFile()){
					linecomepoint = 5;
					dias=0;
					horas=0;
					minutos = 1;
					filgramos1 = 0;
					filgramos2 = 0;
					card.closefile();
					return;
				}
			}
			linecomepoint++;
		}
		extract_data1();
		//card.closefile();
		memset(commandline, '\0', sizeof(commandline) );
	}
	card.closefile();
	
	
	
}
int Listfiles::extract_data_Symplify(void){
	int Symplify_ok =-1;
	sscanf_P( commandline, PSTR("; G-Code generated by Simplify3D(R) Version %d"), &Symplify_ok);
	return Symplify_ok;
}
int Listfiles::extract_data_fromCura(void){
	int Symplify_ok =-1;
	sscanf_P( commandline, PSTR(";Print time: %d day "), &Symplify_ok);
	return Symplify_ok;
}
void Listfiles::extract_data(void){
	dias=-1, horas=-1, minutos -1;
	if (simplify3D == 0){
		sscanf_P( commandline, PSTR(";Print time: %d days %d hours %d minutes"), &dias, &horas, &minutos);
		if(horas == -1){
			sscanf_P( commandline, PSTR(";Print time: %d day %d hours %d minutes"), &dias, &horas, &minutos);
			if(minutos == -1){
				sscanf_P( commandline, PSTR(";Print time: %d day %d hour %d minutes"), &dias, &horas, &minutos);
			}
		}
		else{
			if(minutos == -1){
				sscanf_P( commandline, PSTR(";Print time: %d days %d hour %d minutes"), &dias, &horas, &minutos);
			}
		}
		if(minutos ==-1 && horas ==-1){
			dias = 0;
			sscanf_P( commandline, PSTR(";Print time: %d hours %d minutes"), &horas, &minutos);
		}
		if(horas !=-1 && minutos==-1){
			sscanf_P( commandline, PSTR(";Print time: %d hour %d minutes"), &horas, &minutos);
		}
		if(minutos ==-1){
			dias = 0;
			horas = 0;
			sscanf_P( commandline, PSTR(";Print time: %d minutes"), &minutos);
		}
		if(minutos ==0){
			minutos = 1;
		}
	}
	else if (simplify3D == 1){
		dias = 0;
		sscanf_P( commandline, PSTR(";   Build time: %d hours %d minutes"), &horas, &minutos);
		
		if(minutos==-1){
			
			sscanf_P( commandline, PSTR(";   Build time: %d hours %d minute"), &horas, &minutos);
		}
		if(minutos==-1){
			
			sscanf_P( commandline, PSTR(";   Build time: %d hour %d minutes"), &horas, &minutos);
		}
		if(minutos==-1){
			
			sscanf_P( commandline, PSTR(";   Build time: %d hour %d minute"), &horas, &minutos);
		}
		if(minutos == -1){
			horas = 0;
			minutos = 1;
		}
		
		
	}
	
}
void Listfiles::extract_data1(void){
	
	filgramos1 = 0;
	filgramos2 = 0;
	if(simplify3D == 0){
		sscanf_P(commandline, PSTR(";Filament used: %d.%dm %d.%dg"), &filmetros1, &filmetros2, &filgramos1, &filgramos2);
	}
	else if ( simplify3D == 1){
		sscanf_P(commandline, PSTR(";   Plastic weight: %d.%dg"), &filgramos1, &filgramos2);
	}

}

int Listfiles::get_hours(){
	int hours = 0;
	if(dias>0){
		hours = 24*dias + horas;
	}
	else{
		hours = horas;
	}
	return hours;
}
int Listfiles::get_minutes(){
	int mins = 0;
	mins = minutos;
	return mins;
}
int Listfiles::get_filmetros1(){
	int metr = 0;
	metr = filmetros1;
	return metr;
}
int Listfiles::get_filmetros2(){
	int metr = 0;
	metr = filmetros2;
	return metr;
}
int Listfiles::get_filgramos1(){
	int gram = 0;
	gram = filgramos1;
	return gram;
}
int Listfiles::get_filgramos2(){
	int gram = 0;
	gram = filgramos2;
	return gram;
}
int Listfiles::get_hoursremaining(){
	unsigned long long hours = 0;
	if (get_hours()==0)return 0;
	hours =(unsigned long long)get_hours()*60+(unsigned long long)get_minutes();
	Serial.println((long)hours);
	hours = hours-hours*card.getSdPosition()/card.getFileSize();
	Serial.println((long)hours);
	hours = hours/60;
	Serial.println((long)hours);
	return (int) hours;
}
int Listfiles::get_minutesremaining(){
	unsigned long long minu = 0;
	if (get_minutes()==-1)return 0;
	minu = (unsigned long long)get_hours()*60+(unsigned long long)get_minutes();
	minu = minu-minu*card.getSdPosition()/card.getFileSize();
	minu = minu%60;
	return (int) minu;
}
int Listfiles::get_hoursremaining_save(long position){
	unsigned long long hours = 0;
	if (get_hours()==0)return 0;
	hours =(unsigned long long)get_hours()*60+(unsigned long long)get_minutes();
	Serial.println((long)hours);
	hours = hours-hours*position/card.getFileSize();
	Serial.println((long)hours);
	hours = hours/60;
	Serial.println((long)hours);
	return (int) hours;
}
int Listfiles::get_minutesremaining_save(long position){
	unsigned long long minu = 0;
	if (get_minutes()==-1)return 0;
	minu = (unsigned long long)get_hours()*60+(unsigned long long)get_minutes();
	minu = minu-minu*position/card.getFileSize();
	minu = minu%60;
	return (int) minu;
}
int Listfiles::get_percentage_save(long position){
	unsigned long long minu = 0;
	minu = position*100/card.getFileSize();
	return (int) minu;
}
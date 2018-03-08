#include <allegro.h>
#include "loadpng.h"
#include <cmath>
#include <boost/smart_ptr.hpp>
#include <boost/any.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>

#include "Scanliner.hpp"
#include "XYDistanceShader.hpp"
#include "DistanceShader.hpp"
#include "ShaderEngine.hpp"

#define FPS 20
#define WIDTH 640
#define HEIGHT 480


volatile bool close_button_pressed = false;

void close_button_handler(void) // callback po wciśnięciu krzyżyka
{
	close_button_pressed = true;  // ustaw że został wciśnięty
}
END_OF_FUNCTION(close_button_handler)

static bool lastkey[KEY_MAX]; // zapamietaj że przycisk był juz wciśnięty
bool pressed(int keynum)     // tylko w tedy kiedy wciskamy przycisk
{
	return (!lastkey[keynum]) & (lastkey[keynum]=key[keynum]);
}

void nop(void *x) // nic nierób
{
}

int main(int argc, char *argv[])
{
	if(allegro_init())
	{
		std::clog << "Nie mozna uruchomic allegro";
		return EXIT_FAILURE;
	}
	try
	{
		install_keyboard();
		loadpng_init();

		set_color_depth(32);

		if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, WIDTH, HEIGHT, 0, 0))
			throw "Problem z ustawieniem trybu graficznego";

		LOCK_FUNCTION(close_button_handler);
		set_close_button_callback(close_button_handler);

		std::map< std::string, boost::shared_ptr<BITMAP> > bitmaps;

		bitmaps["screen"] = boost::shared_ptr<BITMAP>(screen,&nop); // ustaw podgląd i niezwalniaj go

		std::istream* in = 0; ///plik z którego otczytujemy

		while(!close_button_pressed && !key[KEY_ESC]) /// dopóki niechcemy wyjść
		{
			if(pressed(KEY_SPACE))  // po naciśnięciu spacji
			{
				if(!in) //jeżeli plik niezostał otwarty
				{
					std::cout << "Otwieram plik run.cfg... " << std::endl;
					in = new std::fstream("run.cfg");
				}
				std::string command,op1,op2;   // ciągi znaków
				while(!in->eof())    // dopuki nie koniec pliku
				{
					if(!(*in >> command)){ // wczytaj polecenie
						continue;   //wyjdzie gdy eof
					}
					if(command[0] == '#') { in->ignore(256,'\n'); continue;} // ignoruj wykomentowane linijki

					if(command == "pause") break;  // podczas pausy wyjdź z pętli wykonywania skryptu

					// ustawiaj różne tryby rysowania
					if(command == "solid"){ solid_mode();std::cout << "Solid bliting mode enabled." << std::endl;continue;}
					if(command == "add"){ set_add_blender(0,0,0,0);std::cout << "Additive  bliting mode enabled." << std::endl;continue;}
					if(command == "alpha"){ set_alpha_blender();std::cout << "Alpha bliting mode enabled." << std::endl;continue;}
					if(command == "writealpha"){ set_write_alpha_blender();std::cout << "Write alpha bliting mode enabled." << std::endl;continue;}

					if(!(*in >> op1)){ // wczytaj 1 operand, jeżeli bląd to
						std::cout << "Brak 1 parametru. " << std::endl;
						break;   //wyjdź
					}

					if(command == "clear")   // polecenie czysc ekran
					{
						int color=0;
						char line[256];
						in->getline(line,256);
						std::sscanf(line,"%x",&color);   // wczytaj kolor w hexah
						if(bitmaps[op1])				 // jeżeli bitmapa z operandu 1 istnieje
							clear_to_color(bitmaps[op1].get(),color); // wyczysc ją
						std::cout << "Wyczyszczono " << op1 << std::endl;
						continue;
					}
					if(!(*in >> op2)){ // wczytaj 2 operand, jeżeli bląd to
						std::cout << "Brak 1 parametru. " << std::endl;
						break;   //wyjdź
					}
					if(command == "load")   // polecenie wczytaj bitmape
					{
						//set_color_conversion(COLORCONV_24_TO_32);
						BITMAP *b = load_bitmap(op1.c_str(),NULL); // odczytaj ją
						if(b)          // jeżeli się wczytała
						{
							bitmaps[op2] = boost::shared_ptr<BITMAP>(b, &destroy_bitmap);  // utwóż inteligentny wskaźnik
							std::cout << "Obraz " << op1 << " załadowany jako " << op2 << std::endl;
						}
						else
						{
							std::cout << "Bład ładowania obrazu " << op1 << std::endl;
						}
					}
					else
					if(command == "save")  // polecenie zapisz bitmape
					{
						//set_color_conversion(COLORCONV_NONE);
						if(bitmaps[op1])    // jeżeli bitmapa z operandu 1 istnieje
						{
							if( save_bitmap(op2.c_str(),bitmaps[op1].get(),NULL) )  //zapisz ją
								std::cout << "Błąd zapisu bitmapy " << op1 << " jako " << op2 << "." << std::endl;
							else
								std::cout << "Obraz " << op1 << " zapisany jako " << op2 << std::endl;
						}
						else
						{
							std::cout << "Niema takiej bitmapy " << op1 << std::endl;
						}
					}
					else
					if(command == "blit")   // polecenie wyświetl
					{
						if(bitmaps[op1] && bitmaps[op2])  // jeżeli operandy wskazują na poprawne bitmapy
						{
							draw_trans_sprite(bitmaps[op2].get(),bitmaps[op1].get(),0,0);  //narysuj
							std::cout << "Wyrysowano " << op1 << " na " << op2 << "." << std::endl;
						}
						else
							std::cout << "Niepoprawne bitmapy." << std::endl;
					}
					else
					if(command == "draw")   // polecenie wyświetl
					{
						if(bitmaps[op1] && bitmaps[op2])  // jeżeli operandy wskazują na poprawne bitmapy
						{
							draw_sprite(bitmaps[op2].get(),bitmaps[op1].get(),0,0);  //narysuj
							std::cout << "Wyrysowano " << op1 << " na " << op2 << "." << std::endl;
						}
						else
							std::cout << "Niepoprawne bitmapy." << std::endl;
					}
					else
					if(command == "XYDistanceShader") // polecenie XYDistanceShader
					{
					    set_write_alpha_blender();    // ustaw rysowanie alpha
					    float cd = 1.;                // moc filtru
						int co = 0xFFFFFF;			// kolor

						char line[32];
						in->getline(line,32);
						std::sscanf(line,"%f %x",&cd,&co); // wczytaj parametry

					    ShaderEngine filter = XYDistanceShader(co,cd); // zainitializuj filtr

						std::cout << "Załadowano filer... ";

						filter( bitmaps[op1] , bitmaps[op2] ); // uruchom filtr

						std::cout << "done. " << std::endl;

					}else
					if(command == "DistanceShader")
					{
					    set_write_alpha_blender();  // ustaw rysowanie alpha
					    float cd = 1; 			   // moc filtru
						int co = 0xFFFFFF;		  // kolor

						char line[32];
						in->getline(line,32);
						std::sscanf(line,"%f %x",&cd,&co);  // wczytaj parametry

					    ShaderEngine filter = DistanceShader(co,cd);  // zainitializuj filtr

						std::cout << "Załadowano filer... ";

						filter(bitmaps[op1],bitmaps[op2]);  // uruchom filtr

						std::cout << "done. " << std::endl;

					}else
						std::cout << "Nieznana komenda " << command << std::endl;
				}
				if(in->eof())
				{
					std::cout << "Skrypt skonczony" << std::endl;
					delete in;
					in=0;
				}
			}
			vsync();
			rest(25);
		}
	}
	catch(char const* msg)
	{
		allegro_message(msg);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
END_OF_MAIN();


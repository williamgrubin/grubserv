#include <stdio.h>

#include <iostream>

#include "protocol.h"
using namespace std;

class version* lurk_version = new version(2,3);
class game* lurk_game = new game(100, 250, 539, (char *)R"(
                      __                                       
                     /\ \                                      
   __   _ __   __  __\ \ \____    ____     __   _ __   __  __  
 /'_ `\/\`'__\/\ \/\ \\ \ '__`\  /',__\  /'__`\/\`'__\/\ \/\ \ 
/\ \L\ \ \ \/ \ \ \_\ \\ \ \L\ \/\__, `\/\  __/\ \ \/ \ \ \_/ |
\ \____ \ \_\  \ \____/ \ \_,__/\/\____/\ \____\\ \_\  \ \___/ 
 \/___L\ \/_/   \/___/   \/___/  \/___/  \/____/ \/_/   \/__/  
   /\____/                                                     
   \_/__/                                                      
)");

class error other_error(0, 12, 		(char *)"other error");
class error bad_room(1, 9, 			(char *)"bad room");
class error player_exists(2, 14, 	(char *)"player exists");
class error bad_monster(3, 12, 		(char *)"bad monster");
class error stat_error(4, 11, 		(char *)"stat error");
class error not_ready(5, 10, 		(char *)"not ready");
class error no_target(6, 10, 		(char *)"no target");
class error no_fight(7, 9, 			(char *)"no fight");
class error no_pvp(8, 7, 			(char *)"no pvp");

class accept accept_message(1);
class accept accept_changeroom(2);
class accept accept_fight(3);
class accept accept_pvp_fight(4);
class accept accept_loot(5);
class accept accept_start(6);
class accept accept_character(10);

class room usg_kellion(0, (char *)"USG Kellion", 161, (char *)"The USG Kellion was dispatched to the USG Ishimura as part of an emergency maintenance mission to determine and repair the cause of the communications blackout.");
class room flight(1, (char *)"Flight Deck", 107, (char *)"The Flight Deck is the location of the main hangers where shuttles would enter and leave the USG Ishimura.");
class room medical(2, (char *)"Medical Deck", 157, (char *)"The Medical Deck is a sprawling location encompassing numerous laboratories, office spaces, storage spaces, and centers for medical diagnosis and treatment.");
class room engineering(3, (char *)"Engineering Deck", 273, (char *)"The Engineering Deck of the USG Ishimura is where various engineers work at maintaining the ship. This area is responsible for ensuring the Ship's Shock Point Drive and gravity centrifuge work, as well as the nav rockets, with a lifting capacity of 525 trillion kilograms.");
class room bridge(4, (char *)"Bridge", 67, (char *)"The Bridge was the command and control center of the USG Ishimura.");
class room hydroponics(5, (char *)"Hydroponics Deck", 186, (char *)"The Hydroponics Deck is the USG Ishimura's internal farm which produces the food needed to feed the many workers aboard the ship as well as providing a sustainable production of oxygen.");
class room food_storage(6, (char *)"Food Storage", 146, (char *)"A survivor's report says that a massive creature entered the Hydroponics deck from outside the ship. That's when the air quality began degrading.");
class room mining(7, (char *)"Mining Deck", 148, (char *)"The Mining Deck  is the heart of the USG Ishimura. This is where asteroids and planet fragments are broken apart and smelted for valuable minerals.");
class room usm_valor(8, (char *)"USM Valor", 217, (char *)"The USM Valor was a Destroyer-class vessel of the Earth Defense Force captained by Commander F. Cadigan. A fifth the size of the USG Ishimura, the Valor possessed formidable firepower and was emblematic of its class.");
class room crew(9, (char *)"Crew Deck", 142, (char *)"The Crew Deck of the USG Ishimura is the residential area for the ship's crew. It is a large relaxing area comprised mainly of sleeper bunks.");
class room aegis(10, (char *)"Aegis VII", 236, (char *)"The Aegis VII Colony was an illegal mining colony on Aegis VII built by the Concordance Extraction Corporation in 2505. After discovering Marker 3A in 2508, the colony descended into madness and was subsequently overrun by Necromorphs.");
class room landing_pad(11, (char *)"Landing Pad", 40, (char *)"Oh... my God. I'm ready. Make me whole.");

// name, damage, defense, regen, health, gold, room, description length, description
class character isaac((char *)"Isaac Clarke", 35, 15, 5, 175, 2008, 0, 36, (char *)"Stick around. I'm full of bad ideas.");

class character flight_slasher((char *)"Slasher", 20, 5, 5, 40, 5, 1, 1, (char *)"");
class character flight_leaper((char *)"Leaper", 15, 5, 5, 25, 5, 1, 1, (char *)"");

class character medical_slasher((char*)"Slasher", 20, 5, 5, 40, 5, 2, 1, (char *)"");
class character medical_enhanced_slasher((char *)"Enhanced Slasher", 35, 5, 5, 60, 15, 2, 1, (char *)"");
class character medical_leaper((char *)"Leaper", 15, 5, 5, 25, 5, 2, 1, (char *)"");
class character medical_lurker((char *)"Lurker", 10, 5, 5, 15, 5, 2, 1, (char *)"");
class character medical_infector((char *)"Infector", 5, 5, 5, 10, 5, 2, 1, (char *)"");
class character medical_swarmers((char *)"Swarmers", 15, 0, 0, 10, 5, 2, 1, (char *)"");

class character engineering_slasher((char*)"Slasher", 20, 5, 5, 40, 5, 3, 1, (char *)"");
class character engineering_enhanced_slasher((char *)"Enhanced Slasher", 35, 5, 5, 60, 15, 3, 1, (char *)"");
class character engineering_pregnant((char *)"Pregnant", 25, 5, 5, 50, 5, 3, 1, (char *)"");
class character engineering_leaper((char *)"Leaper", 15, 5, 5, 25, 5, 3, 1, (char *)"");
class character engineering_lurker((char *)"Lurker", 10, 5, 5, 15, 5, 3, 1, (char *)"");
class character engineering_infector((char *)"Infector", 5, 5, 5, 10, 5, 3, 1, (char *)"");
class character engineering_tentacle((char *)"Tentacle", 10, 5, 5, 50, 5, 3, 1, (char *)"");
class character engineering_swarmers((char *)"Swarmers", 15, 0, 0, 10, 5, 3, 1, (char *)"");

class character bridge_slasher((char*)"Slasher", 20, 5, 5, 40, 5, 4, 1, (char *)"");
class character bridge_enhanced_slasher((char *)"Enhanced Slasher", 35, 5, 5, 60, 15, 4, 1, (char *)"");
class character bridge_brute((char *)"Brute", 45, 15, 5, 50, 5, 4, 1, (char *)"");
class character bridge_leaper((char *)"Leaper", 15, 5, 5, 25, 5, 4, 1, (char *)"");
class character bridge_lurker((char *)"Lurker", 10, 5, 5, 15, 5, 4, 1, (char *)"");
class character bridge_infector((char *)"Infector", 5, 5, 5, 10, 5, 4, 1, (char *)"");
class character bridge_divider((char *)"Divider", 10, 5, 5, 20, 5, 4, 1, (char *)"");

class character hydroponics_slasher((char*)"Slasher", 20, 5, 5, 40, 5, 5, 1, (char *)"");
class character hydroponics_pregnant((char *)"Pregnant", 25, 5, 5, 50, 5, 5, 1, (char *)"");
class character hydroponics_lurker((char *)"Lurker", 10, 5, 5, 15, 5, 5, 1, (char *)"");
class character hydroponics_hunter((char *)"Hunter", 20, 5, 5, 50, 5, 5, 1, (char *)"");
class character hydroponics_guardian((char *)"Guardian", 30, 5, 5, 15, 5, 5, 1, (char *)"");
class character hydroponics_swarmers((char *)"Swarmers", 15, 0, 0, 10, 5, 5, 1, (char *)"");

class character food_storage_slasher((char*)"Slasher", 20, 5, 5, 40, 5, 6, 1, (char *)"");
class character food_storage_enhanced_slasher((char *)"Enhanced Slasher", 35, 5, 5, 60, 15, 6, 1, (char *)"");
class character food_storage_pregnant((char *)"Pregnant", 25, 5, 5, 50, 5, 6, 1, (char *)"");
class character food_storage_brute((char *)"Brute", 45, 15, 5, 50, 5, 6, 1, (char *)"");
class character food_storage_lurker((char *)"Lurker", 10, 5, 5, 15, 5, 6, 1, (char *)"");
class character food_storage_exploder((char *)"Exploder", 45, 0, 0, 10, 5, 6, 1, (char *)"");
class character food_storage_wheezer((char *)"Wheezer", 0, 0, 0, 5, 5, 6, 1, (char *)"");
class character leviathan((char *)"The Leviathan", 50, 5, 0, 100, 25, 6, 1, (char *)"");
class character food_storage_guardian((char *)"Guardian", 30, 5, 5, 15, 5, 6, 1, (char *)"");
class character food_storage_swarmers((char *)"Swarmers", 15, 0, 0, 10, 5, 6, 1, (char *)"");

class character mining_deck_slasher((char*)"Slasher", 20, 5, 5, 40, 5, 7, 1, (char *)"");
class character mining_deck_enhanced_slasher((char *)"Enhanced Slasher", 35, 5, 5, 60, 15, 7, 1, (char *)"");
class character mining_deck_pregnant((char *)"Pregnant", 25, 5, 5, 50, 5, 7, 1, (char *)"");
class character mining_deck_lurker((char *)"Lurker", 10, 5, 5, 15, 5, 7, 1, (char *)"");
class character mining_deck_exploder((char *)"Exploder", 45, 0, 0, 10, 5, 7, 1, (char *)"");
class character mining_deck_infector((char *)"Infector", 5, 5, 5, 10, 5, 7, 1, (char *)"");
class character mining_deck_pod((char *)"Pod", 15, 0, 0, 15, 5, 7, 1, (char *)"");
class character mining_deck_swarmers((char *)"Swarmers", 15, 0, 0, 10, 5, 7, 1, (char *)"");
class character mining_deck_divider((char *)"Divider", 10, 5, 5, 20, 5, 7, 1, (char *)"");

class character usm_valor_leaper((char *)"Leaper", 15, 5, 5, 25, 5, 8, 1, (char *)"");
class character usm_valor_enhanced_leaper((char *)"Enhanced Leaper", 15, 5, 5, 40, 5, 8, 1, (char *)"");
class character usm_valor_enhanced_brute((char *)"Enhanced Brute", 50, 15, 5, 65, 5, 8, 1, (char *)"");
class character usm_valor_pregnant((char *)"Pregnant", 25, 5, 5, 50, 5, 8, 1, (char *)"");
class character usm_valor_twitcher((char *)"Twitcher", 25, 10, 0, 35, 10, 8, 1, (char *)"");
class character usm_valor_divider((char *)"Divider", 10, 5, 5, 20, 5, 8, 1, (char *)"");
class character usm_valor_lurker((char *)"Lurker", 10, 5, 5, 15, 5, 8, 1, (char *)"");
class character usm_valor_exploder((char *)"Exploder", 45, 0, 0, 10, 5, 8, 1, (char *)"");
class character usm_valor_infector((char *)"Infector", 5, 5, 5, 10, 5, 8, 1, (char *)"");

class character crew_deck_slasher((char*)"Slasher", 20, 5, 5, 40, 5, 9, 1, (char *)"");
class character crew_deck_enhanced_slasher((char *)"Enhanced Slasher", 35, 5, 5, 60, 15, 9, 1, (char *)"");
class character crew_deck_pregnant((char *)"Pregnant", 25, 5, 5, 50, 5, 9, 1, (char *)"");
class character crew_deck_twitcher((char *)"Twitcher", 25, 10, 0, 35, 10, 9, 1, (char *)"");
class character crew_deck_divider((char *)"Divider", 10, 5, 5, 20, 5, 9, 1, (char *)"");
class character crew_deck_exploder((char *)"Exploder", 45, 0, 0, 10, 5, 9, 1, (char *)"");
class character crew_deck_infector((char *)"Infector", 5, 5, 5, 10, 5, 9, 1, (char *)"");
class character crew_deck_hunter((char *)"Hunter", 20, 5, 5, 50, 5, 9, 1, (char *)"");
class character crew_deck_tentacle((char *)"Tentacle", 10, 5, 5, 50, 5, 9, 1, (char *)"");
class character crew_deck_guardian((char *)"Guardian", 30, 5, 5, 15, 5, 9, 1, (char *)"");
class character crew_deck_pod((char *)"Pod", 15, 0, 0, 15, 5, 9, 1, (char *)"");
class character crew_deck_swarmers((char *)"Swarmers", 15, 0, 0, 10, 5, 9, 1, (char *)"");

class character aegis_slasher((char *)"Enhanced Slasher", 35, 5, 5, 60, 15, 10, 1, (char *)"");
class character aegis_enhanced_leaper((char *)"Enhanced Leaper", 15, 5, 5, 40, 5, 10, 1, (char *)"");
class character aegis_enhanced_brute((char *)"Enhanced Brute", 50, 15, 5, 65, 5, 10, 1, (char *)"");
class character aegis_lurker((char *)"Lurker", 10, 5, 5, 15, 5, 10, 1, (char *)"");
class character aegis_enhanced_lurker((char *)"Lurker", 15, 5, 5, 25, 5, 10, 1, (char *)"");
class character aegis_pregnant((char *)"Pregnant", 25, 5, 5, 50, 5, 10, 1, (char *)"");
class character aegis_twitcher((char *)"Twitcher", 25, 10, 0, 35, 10, 10, 1, (char *)"");
class character aegis_divider((char *)"Divider", 10, 5, 5, 20, 5, 10, 1, (char *)"");
class character aegis_exploder((char *)"Exploder", 45, 0, 0, 10, 5, 10, 1, (char *)"");
class character aegis_infector((char *)"Infector", 5, 5, 5, 10, 5, 10, 1, (char *)"");
class character aegis_tentacle((char *)"Tentacle", 10, 5, 5, 50, 5, 10, 1, (char *)"");
class character aegis_guardian((char *)"Guardian", 30, 5, 5, 15, 5, 10, 1, (char *)"");

class character hive_mind((char *)"The Hive Mind", 45, 0, 0, 250, 1000, 11, 1, (char *)"");

class start begin_game;

// store room and character objects as pointers (*) inside of these vectors in order to prevent destructor issues
vector<error*> errors;
vector<room*> rooms;
map<int, character*> character_map;
// map<uint16_t, vector<uint16_t>> connection_map;
vector<character*> npcs;

void storeObjects() {
    npcs.push_back(&isaac);

	errors.push_back(&other_error);
	errors.push_back(&bad_room);
	errors.push_back(&player_exists);
	errors.push_back(&bad_monster);
	errors.push_back(&stat_error);
	errors.push_back(&not_ready);
	errors.push_back(&no_target);
	errors.push_back(&no_fight);
	errors.push_back(&no_pvp);

	rooms.push_back(&usg_kellion);
	rooms.push_back(&flight);
	rooms.push_back(&medical);
	rooms.push_back(&engineering);
	rooms.push_back(&bridge);
	rooms.push_back(&hydroponics);
	rooms.push_back(&food_storage);
	rooms.push_back(&mining);
	rooms.push_back(&usm_valor);
	rooms.push_back(&crew);
	rooms.push_back(&aegis);
	rooms.push_back(&landing_pad);

	npcs.push_back(&flight_slasher);
	npcs.push_back(&flight_leaper);

	npcs.push_back(&medical_slasher);
	npcs.push_back(&medical_enhanced_slasher);
	npcs.push_back(&medical_leaper);
	npcs.push_back(&medical_lurker);
	npcs.push_back(&medical_infector);
	npcs.push_back(&medical_swarmers);

	npcs.push_back(&engineering_slasher);
	npcs.push_back(&engineering_enhanced_slasher);
	npcs.push_back(&engineering_pregnant);
	npcs.push_back(&engineering_leaper);
	npcs.push_back(&engineering_lurker);
	npcs.push_back(&engineering_infector);
	npcs.push_back(&engineering_tentacle);
	npcs.push_back(&engineering_swarmers);

	npcs.push_back(&bridge_slasher);
	npcs.push_back(&bridge_enhanced_slasher);
	npcs.push_back(&bridge_brute);
	npcs.push_back(&bridge_leaper);
	npcs.push_back(&bridge_lurker);
	npcs.push_back(&bridge_infector);
	npcs.push_back(&bridge_divider);

	npcs.push_back(&hydroponics_slasher);
	npcs.push_back(&hydroponics_pregnant);
	npcs.push_back(&hydroponics_lurker);
	npcs.push_back(&hydroponics_hunter);
	npcs.push_back(&hydroponics_guardian);
	npcs.push_back(&hydroponics_swarmers);

	npcs.push_back(&food_storage_slasher);
	npcs.push_back(&food_storage_enhanced_slasher);
	npcs.push_back(&food_storage_pregnant);
	npcs.push_back(&food_storage_brute);
	npcs.push_back(&food_storage_lurker);
	npcs.push_back(&food_storage_exploder);
	npcs.push_back(&food_storage_wheezer);
	npcs.push_back(&leviathan);
	npcs.push_back(&food_storage_guardian);
	npcs.push_back(&food_storage_swarmers);

	npcs.push_back(&mining_deck_slasher);
	npcs.push_back(&mining_deck_enhanced_slasher);
	npcs.push_back(&mining_deck_pregnant);
	npcs.push_back(&mining_deck_lurker);
	npcs.push_back(&mining_deck_exploder);
	npcs.push_back(&mining_deck_infector);
	npcs.push_back(&mining_deck_pod);
	npcs.push_back(&mining_deck_swarmers);
	npcs.push_back(&mining_deck_divider);

	npcs.push_back(&usm_valor_leaper);
	npcs.push_back(&usm_valor_enhanced_leaper);
	npcs.push_back(&usm_valor_enhanced_brute);
	npcs.push_back(&usm_valor_pregnant);
	npcs.push_back(&usm_valor_twitcher);
	npcs.push_back(&usm_valor_divider);
	npcs.push_back(&usm_valor_lurker);
	npcs.push_back(&usm_valor_exploder);
	npcs.push_back(&usm_valor_infector);

	npcs.push_back(&crew_deck_slasher);
	npcs.push_back(&crew_deck_enhanced_slasher);
	npcs.push_back(&crew_deck_pregnant);
	npcs.push_back(&crew_deck_twitcher);
	npcs.push_back(&crew_deck_divider);
	npcs.push_back(&crew_deck_exploder);
	npcs.push_back(&crew_deck_infector);
	npcs.push_back(&crew_deck_hunter);
	npcs.push_back(&crew_deck_tentacle);
	npcs.push_back(&crew_deck_guardian);
	npcs.push_back(&crew_deck_pod);
	npcs.push_back(&crew_deck_swarmers);

	npcs.push_back(&aegis_slasher);
	npcs.push_back(&aegis_enhanced_leaper);
	npcs.push_back(&aegis_enhanced_brute);
	npcs.push_back(&aegis_lurker);
	npcs.push_back(&aegis_enhanced_lurker);
	npcs.push_back(&aegis_pregnant);
	npcs.push_back(&aegis_twitcher);
	npcs.push_back(&aegis_divider);
	npcs.push_back(&aegis_exploder);
	npcs.push_back(&aegis_infector);
	npcs.push_back(&aegis_tentacle);
	npcs.push_back(&aegis_guardian);

	npcs.push_back(&hive_mind);
}
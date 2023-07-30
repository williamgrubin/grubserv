#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<bits/stdc++.h>
#include<cstring>
#include<mutex>
#include<thread>
#include<vector>
#include<map>
#include<string>
#include<algorithm>

#include "protocol.h"
using namespace std;

#define MESSAGE 1
#define CHANGEROOM 2
#define FIGHT 3
#define PVP 4
#define LOOT 5
#define START 6
#define CHARACTER 10
#define LEAVE 12

#define our character_map[file_descriptor]

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

int skt;
vector<client*> clients;
mutex clients_mutex;
mutex server_mutex;

// store room and character objects as pointers (*) inside of these vectors in order to prevent destructor issues
vector<error*> errors;
vector<room*> rooms;
map<int, character*> character_map;
// map<uint16_t, vector<uint16_t>> connection_map;
vector<character*> npcs;

int disconnected_players = 0;

int disconnect_character(int fd, map<int, character*> *characters) {
	clients_mutex.lock();
	printf("Disconnecting character with descriptor: %d.\n", fd);
	// if the player is already disconnected
	if(fd < 0) {
		printf("Player \"%s\" appears to have already been disconnected.\nReturning to regular operations...\n", character_map[fd]->player_name);
		clients_mutex.unlock();
		return fd;
	}
	auto map_node = characters->extract(fd);
	if(map_node) {
		disconnected_players--;
		map_node.key() = disconnected_players;
		characters->insert(move(map_node));
		character_map.erase(fd);
		clients_mutex.unlock();
		return disconnected_players;
	}
	clients_mutex.unlock();
	return disconnected_players;
}

void update_character(class character* us, map<int, character*> *them) {
	for(auto i : *them) {
		if(!i.second) {
			character_map.erase(i.first);
			continue;
		}
		if(i.second->current_room == us->current_room) {
			if(i.first > 0) {
				if(!us->send(i.first)) {
					i.second->alive = 0;
					i.second->started = 0;
					continue;
				}
			}
		}
	}
	return;
}

void cleanup_map() {
	clients_mutex.lock();
	for(auto& i : character_map) {
		if(!i.second) {
			character_map.erase(i.first);
		}
	}
	clients_mutex.unlock();
}

void cleanup_clients() {
	clients_mutex.lock();
	for(int i = 0; i < clients.size(); i++) {
		if(clients[i]->file_descriptor < 0) {
			clients[i]->t.join();
			delete clients[i];
			clients[i] = clients[clients.size() - 1];
			clients.pop_back();
		}
	}
	clients_mutex.unlock();
}

void cleanup_server() {
	server_mutex.lock();
	for(auto i : errors) { delete(i); }
	for(auto j : rooms) { delete(j); }
	for(auto k : character_map) {
		printf("Deleting CHARACTER \"%s\".\n", k.second->player_name);
		delete(k.second);
		character_map.erase(k.first);
	}
	for(auto x : npcs) {
		printf("Deleting NPC \"%s\".\n", x->player_name);
		delete(x);
	}
	delete(lurk_game);
	server_mutex.unlock();
	cleanup_clients();
	return;
}

void handle_signal(int signal) {
	if(signal == 13) {
		printf("Received a SIGPIPE...\n");
		// cleanup_clients();
		return;
	}
	if(signal == 2) {
		printf("Received a SIGINT...\n");
		cleanup_server();
		close(skt);
		exit(0);
	}
	return;
}

bool is_dead(character* client_character, map<int, character*> existing_characters) {
	if(existing_characters.size() < 1) { return false; }
	for(auto& i : existing_characters) {
		if(i.second && client_character) { if(!strcmp(client_character->player_name, i.second->player_name) && i.second->alive == 0 && i.first < 0) { return true; } }
	}
	return false;
}

bool is_duplicate(int client_descriptor, character* client_character, map<int, character*> existing_characters) {
	if(existing_characters.size() < 1) { return false; }
	for(auto& i : existing_characters) {
		if(i.second && client_character) { if(!strcmp(client_character->player_name, i.second->player_name)) { return true; } }
	}
	return false;
}

bool is_overpowered(character* client_character, game* this_game) {
	if(client_character->attack_damage + client_character->defense_rating + client_character->regeneration > this_game->initial_points) { return true; }
	return false;
}

void client::run() {
	uint8_t type;
	while(1) {
		if(character_map.size() > 1 && character_map.find(file_descriptor) != character_map.end()) { cleanup_map(); }
		if(-1 == recv(file_descriptor, &type, 1, MSG_PEEK)) {
			write_mutex.lock();
			close(file_descriptor);
			if(our) {
				printf("Player \"%s\" has disconnected ungracefully...\n", our->player_name);
				our->alive = 0;
				our->started = 0;
				file_descriptor = disconnect_character(file_descriptor, &character_map);
				update_character(our, &character_map);
			}
			file_descriptor = -1;
			write_mutex.unlock();
			break;
		}

		if(character_map.find(file_descriptor) != character_map.end()) {
			if(type == CHANGEROOM && our->alive) {
				write_mutex.lock();
				changeroom *temp_changeroom = new changeroom();
				uint16_t source_room_number = our->current_room;
				if(!temp_changeroom->receive(file_descriptor)) {
					printf("CHANGEROOM receive() from descriptor %d failed!\n", file_descriptor);
					close(file_descriptor);
					file_descriptor = disconnect_character(file_descriptor, &character_map);
					our->alive = 0;
					our->started = 0;
					update_character(our, &character_map);
					write_mutex.unlock();
					break;
				}
				if(temp_changeroom->room_number > (our->current_room + 1) || temp_changeroom->room_number < (our->current_room - 1) || temp_changeroom->room_number > 11) {
					if(!bad_room.send(file_descriptor)) {
						close(file_descriptor);
						file_descriptor = disconnect_character(file_descriptor, &character_map);
						our->alive = 0;
						our->started = 0;
						update_character(our, &character_map);
						write_mutex.unlock();
						break;
					}
					write_mutex.unlock();
					continue;
				}
				// update our CHARACTER.room_number
				our->current_room = temp_changeroom->room_number;
				update_character(our, &character_map);
				if(our && character_map.find(file_descriptor) != character_map.end()) {
					if(!temp_changeroom->send(file_descriptor, rooms[our->current_room], rooms[temp_changeroom->room_number], our, character_map, npcs, rooms)) {
						// disconnect the client
						close(file_descriptor);
						file_descriptor = disconnect_character(file_descriptor, &character_map);
						our->alive = 0;
						our->started = 0;
						update_character(our, &character_map);
						write_mutex.unlock();
						break;
					} else {
						if(!accept_changeroom.send(file_descriptor)) {
							close(file_descriptor);
							file_descriptor = disconnect_character(file_descriptor, &character_map);
							our->alive = 0;
							our->started = 0;
							update_character(our, &character_map);
							write_mutex.unlock();
							break;
						}
					}
				}
				if(character_map.find(file_descriptor) != character_map.end()) {
					if(character_map[file_descriptor]->join_battle) {
						for(auto i : npcs) {
							if(i->current_room == our->current_room) {
								while(i->alive && our->alive) {
									i->health_points -= (our->attack_damage);
									i->health_points += (i->regeneration);
									our->health_points -= (i->attack_damage);
									our->health_points += (our->regeneration);
									if(i->health_points == 0 || i->health_points > lurk_game->stat_limit) { i->alive = 0; }
									if(our->health_points == 0 || our->health_points > lurk_game->stat_limit) { our->alive = 0; }
									update_character(i, &character_map);
									update_character(our, &character_map);
								}
							}
						}
					}
				}
				write_mutex.unlock();
				continue;
			}

			if(type == FIGHT && our->alive) {
				write_mutex.lock();
				if(!fight().receive(file_descriptor)) {
					close(file_descriptor);
					file_descriptor = disconnect_character(file_descriptor, &character_map);
					our->alive = 0;
					our->started = 0;
					update_character(our, &character_map);
					write_mutex.unlock();
					break;
				}
				for(auto& i : npcs) {
					if(i->current_room == our->current_room && i->alive) {
						i->health_points -= (our->attack_damage);
						i->health_points += (i->regeneration);
						our->health_points -= (i->attack_damage);
						our->health_points += (our->regeneration);
						if(i->health_points == 0 || i->health_points > lurk_game->stat_limit) { i->health_points = 0; i->alive = 0; }
						if(our->health_points == 0 || our->health_points > lurk_game->stat_limit) { our->health_points = 0; our->alive = 0; }
						update_character(i, &character_map);
						update_character(our, &character_map);
					}
				}
				write_mutex.unlock();
				continue;
			}

			if(type == PVP && our->alive) {
				write_mutex.lock();
				class pvp_fight *temp_fight = new pvp_fight();
				if(!temp_fight->receive(file_descriptor)) {
					close(file_descriptor);
					file_descriptor = disconnect_character(file_descriptor, &character_map);
					our->alive = 0;
					our->started = 0;
					update_character(our, &character_map);
					write_mutex.unlock();
					break;
				}
				if(character_map.size() > 1) {
					for(auto& i : character_map) {
						if(i.second && our) {
							if(!strcmp(i.second->player_name, temp_fight->player_name) && strcmp(our->player_name, temp_fight->player_name)) {
								if(i.second->current_room == our->current_room && i.second->alive) {
									i.second->health_points -= (our->attack_damage);
									i.second->health_points += (i.second->regeneration);
									our->health_points -= (i.second->attack_damage);
									our->health_points += (our->regeneration);
									if(i.second->health_points == 0 || i.second->health_points > lurk_game->stat_limit) { i.second->health_points = 0; i.second->alive = 0; }
									if(our->health_points == 0 || our->health_points > lurk_game->stat_limit) { our->health_points = 0; our->alive = 0; }
									update_character(i.second, &character_map);
									update_character(our, &character_map);
								}
							}
						}
					}
				}
				write_mutex.unlock();
				continue;
			}

			if(type == LOOT && our->alive) {
				write_mutex.lock();
				bool found = false;
				class loot* tmp = new loot();
				if(!tmp->receive(file_descriptor)) {
					close(file_descriptor);
					file_descriptor = disconnect_character(file_descriptor, &character_map);
					our->alive = 0;
					our->started = 0;
					update_character(our, &character_map);
					write_mutex.unlock();
					break;
				}
				printf("Looking for CHARACTER \"%s\" so that \"%s\" can LOOT them...\n", tmp->player_name, our->player_name);
				for(auto& i : character_map) {
					if(!strcmp(i.second->player_name, tmp->player_name) && !i.second->alive) {
						our->gold_count += i.second->gold_count;
						i.second->gold_count = 0;
						update_character(our, &character_map);
						update_character(i.second, &character_map);
						found = true;
						printf("Found the dead player!\n");
					}
					if(found) { break; }
				}
				if(found) {
					write_mutex.unlock();
					continue;
				}
				printf("Looking for MONSTER \"%s\" so that \"%s\" can LOOT them...\n", tmp->player_name, our->player_name);
				for(auto& j : npcs) {
					if(!strcmp(j->player_name, tmp->player_name) && !j->alive) {
						our->gold_count += j->gold_count;
						j->gold_count = 0;
						update_character(our, &character_map);
						update_character(j, &character_map);
						found = true;
						printf("Found the dead monster!\n");
					}
					if(found) { break; }
				}
				if(!found) {
					if(!no_target.send(file_descriptor)) {
						close(file_descriptor);
						file_descriptor = disconnect_character(file_descriptor, &character_map);
						break;
					}
				}
				write_mutex.unlock();
				continue;
			}

			if(type == MESSAGE) {
				write_mutex.lock();
				printf("Player \"%s\" is sending a message...\n", our->player_name);
				class message* temp_message = new message();
				class error* message_error = new error();
				int recipient_descriptor = -1;
				if(!temp_message->receive(file_descriptor, character_map)) {
					printf("MESSAGE receive() from descriptor %d failed!\n", file_descriptor);
					close(file_descriptor);
					file_descriptor = disconnect_character(file_descriptor, &character_map);
					our->alive = 0;
					our->started = 0;
					update_character(our, &character_map);
					delete(temp_message);
					delete(message_error);
					write_mutex.unlock();
					break;
				}
				for(auto& i : character_map) {
					if(i.second) {
						if(!strcmp(i.second->player_name, temp_message->recipient_name)) {
							recipient_descriptor = i.first;
							printf("Found the correct recipient descriptor: %d.\n", recipient_descriptor);
							break;
						}
					}
				}
				if(recipient_descriptor < 0) {
					message_error->error_code = 6;
					message_error->error_length = 63;
					message_error->error_message = (char *)malloc(message_error->error_length);
					strncpy(message_error->error_message, (char *)"Attempt to send MESSAGE to disconnected or nonesxistent player", message_error->error_length);
					if(!message_error->send(file_descriptor)) {
						close(file_descriptor);
						file_descriptor = disconnect_character(file_descriptor, &character_map);
						our->alive = 0;
						our->started = 0;
						update_character(our, &character_map);
						delete(temp_message);
						delete(message_error);
						write_mutex.unlock();
						break;
					}
					delete(temp_message);
					delete(message_error);
					write_mutex.unlock();
					continue;
				} else {
					if(!temp_message->send(recipient_descriptor)) {
						close(recipient_descriptor);
						recipient_descriptor = disconnect_character(recipient_descriptor, &character_map);
						character_map[recipient_descriptor]->alive = 0;
						character_map[recipient_descriptor]->started = 0;
						update_character(character_map[recipient_descriptor], &character_map);
						delete(temp_message);
						write_mutex.unlock();
						break;
					}
					if(!accept_message.send(file_descriptor)) {
						close(file_descriptor);
						file_descriptor = disconnect_character(file_descriptor, &character_map);
						our->alive = 0;
						our->started = 0;
						update_character(our, &character_map);
						delete(temp_message);
						write_mutex.unlock();
						break;
					}
				}
				delete(temp_message);
				write_mutex.unlock();
				continue;
			}

			if(type == START) {
				write_mutex.lock();
				class error* start_error = new error();
				if(1 != read(file_descriptor, &type, 1)) {
					close(file_descriptor);
					file_descriptor = disconnect_character(file_descriptor, &character_map);
					our->alive = 0;
					our->started = 0;
					update_character(our, &character_map);
					write_mutex.unlock();
					break;
				}
				// if the client hasn't created a valid CHARACTER
				if(character_map.find(file_descriptor) == character_map.end()) {
					start_error->error_code = 0;
					start_error->error_length = 44;
					start_error->error_message = (char *)malloc(start_error->error_length);
					strncpy(start_error->error_message, (char *)"Attempted START without a valid CHARACTER\n", start_error->error_length);
					if(!start_error->send(file_descriptor)) {
						close(file_descriptor);
						file_descriptor = -1;
						delete(start_error);
						write_mutex.unlock();
						break;
					}
					delete(start_error);
					write_mutex.unlock();
					continue;
				}
				// check if they've already started
				if(our && our->started == 1) {
					start_error->error_code = 0;
					start_error->error_length = 39;
					start_error->error_message = (char *)malloc(start_error->error_length);
					strncpy(start_error->error_message, (char *)"Attempted START after already started\n", start_error->error_length);
					if(!start_error->send(file_descriptor)) {
						close(file_descriptor);
						file_descriptor = disconnect_character(file_descriptor, &character_map);
						our->alive = 0;
						our->started = 0;
						update_character(our, &character_map);
						delete(start_error);
						write_mutex.unlock();
						break;
					}
					delete(start_error);
					write_mutex.unlock();
					continue;
				}
				// if neither, then start their game
				if(!begin_game.send(file_descriptor, rooms[our->current_room], our, character_map, npcs)) {
					if(our) { printf("START failed for client %d with player \"%s\"\n", file_descriptor, our->player_name); }
					close(file_descriptor);
					file_descriptor = disconnect_character(file_descriptor, &character_map);
					our->alive = 0;
					our->started = 0;
					update_character(our, &character_map);
					write_mutex.unlock();
					break;
				} else {
					if(!accept_start.send(file_descriptor)) {
						close(file_descriptor);
						file_descriptor = disconnect_character(file_descriptor, &character_map);
						our->alive = 0;
						our->started = 0;
						update_character(our, &character_map);
						write_mutex.unlock();
						break;
					}
				}
				// update our CHARACTER's STARTED flag
				our->started = 1;
				for(auto& i : rooms) {
					if(i->room_number == (our->current_room + 1) || i->room_number == (our->current_room - 1)) {
						if(!i->send_connection(file_descriptor)) {
							close(file_descriptor);
							file_descriptor = disconnect_character(file_descriptor, &character_map);
							our->alive = 0;
							our->started = 0;
							update_character(our, &character_map);
							write_mutex.unlock();
							break;
						}
					}
				}
				write_mutex.unlock();
				continue;
			}
		}

		if(type == LEAVE) {
			write_mutex.lock();
			uint8_t temp_buffer;
			if(character_map.find(file_descriptor) != character_map.end()) {
				printf("Player \"%s\" gracefully disconnecting from grubserv...\n", our->player_name);
				string leave_message = "Player \"" + (string)our->player_name + "\" has left the game in room " + to_string(our->current_room) + ".";
				class message* leave_notification = new message((uint16_t)(leave_message.length() + 1), (char *)"Grubin", (char *)"Grubin", true, (char *)leave_message.c_str());
				our->alive = 0;
				our->started = 0;
				update_character(our, &character_map);
				for(auto& i : character_map) {
					if(i.first > 0 && strcmp(i.second->player_name, our->player_name)) { 
						strncpy(leave_notification->recipient_name, i.second->player_name, 32);
						if(!leave_notification->send(i.first)) {
							close(i.first);
							i.second->alive = 0;
							i.second->started = 0;
							update_character(i.second, &character_map);
							continue;
						}
					}
				}
				delete(leave_notification);
			}
			close(file_descriptor);
			file_descriptor = disconnect_character(file_descriptor, &character_map);
			write_mutex.unlock();
			break;
		}

		if(type == CHARACTER) {
			write_mutex.lock();
			// create a temporary CHARACTER
			character *temp_character = new character();
			if(!temp_character->receive(file_descriptor)) {
				printf("Failed to receive() client %d's CHARACTER.\n", file_descriptor);
				close(file_descriptor);
				if(our) {
					file_descriptor = disconnect_character(file_descriptor, &character_map);
					our->alive = 0;
					our->started = 0;
					update_character(our, &character_map);
				} else {
					disconnected_players--;
					file_descriptor = disconnected_players;
				}
				write_mutex.unlock();
				// then start over
				continue;
			}
			// check if the client's CHARACTER stats are valid
			if(is_overpowered(temp_character, lurk_game)) {
				if(!stat_error.send(file_descriptor)) {
					close(file_descriptor);
					if(our) {
						file_descriptor = disconnect_character(file_descriptor, &character_map);
						our->alive = 0;
						our->started = 0;
						update_character(our, &character_map);
					} else {
						disconnected_players--;
						file_descriptor = disconnected_players;
					}
					write_mutex.unlock();
					break;
				}
				write_mutex.unlock();
				continue;
			}
			// if a client has tried to create a previously disconnected/deceased CHARACTER
			if(is_duplicate(file_descriptor, temp_character, character_map) && !is_dead(temp_character, character_map)) {
				// tell the client
				if(!player_exists.send(file_descriptor)) {
					close(file_descriptor);
					if(our) {
						file_descriptor = disconnect_character(file_descriptor, &character_map);
						our->alive = 0;
						our->started = 0;
						update_character(our, &character_map);
					}
					file_descriptor = -1;
					write_mutex.unlock();
					break;
				}
				write_mutex.unlock();
				continue;
			} else if(is_duplicate(file_descriptor, temp_character, character_map) && is_dead(temp_character, character_map)) {
				int found = false;
				string reconnect_message = "Player \"" + (string)temp_character->player_name + "\" has reconnected.";
				class message* reconnect_notification = new message((uint16_t)(reconnect_message.length() + 1), (char *)"Grubin", (char *)"Grubin", true, (char *)reconnect_message.c_str());
				disconnected_players++;
				// loop through each CHARACTER
				for(auto& i : character_map) {
					// find our disconnected/deceased CHARACTER
					if(!strcmp(i.second->player_name, temp_character->player_name) && i.first < 0) {
						found = true;
						// if the player we're rejoining the game as didn't die
						if(i.second->health_points > 0 && i.second->health_points < lurk_game->stat_limit) {
							// put us back to where we were at when we left
							temp_character->join_battle 		= i.second->join_battle;
							temp_character->attack_damage 		= i.second->attack_damage;
							temp_character->defense_rating 		= i.second->defense_rating;
							temp_character->regeneration 		= i.second->regeneration;
							temp_character->health_points 		= i.second->health_points;
							temp_character->gold_count 			= i.second->gold_count;
							temp_character->current_room 		= i.second->current_room;
							temp_character->description_length 	= i.second->description_length;
							strcpy(temp_character->player_description, i.second->player_description);
						}						
						// delete our original (previously disconnected) pair
						free(character_map[i.first]->player_description);
						character_map.erase(i.first);
					}
					if(found) { break; }
				}
				// for each CHARACTER
				for(auto& i : character_map) {
					if(i.first > 0 && i.second) {
						strncpy(reconnect_notification->recipient_name, i.second->player_name, 32);
						if(!reconnect_notification->send(i.first)) {
							close(i.first);
							i.second->alive = 0;
							i.second->started = 0;
							update_character(i.second, &character_map);
							write_mutex.unlock();
							continue;
						}
					}
				}
				delete(reconnect_notification);
			}
			// tell the client their CHARACTER is valid
			if(!accept_character.send(file_descriptor)) {
				close(file_descriptor);
				file_descriptor = disconnect_character(file_descriptor, &character_map);
				file_descriptor = disconnected_players;
				our->alive = 0;
				our->started = 0;
				update_character(our, &character_map);
				write_mutex.unlock();
				break;
			}
			// if our character doesn't already exist in the map
			if(character_map.find(file_descriptor) == character_map.end()) {
				// add our character to the map
				character_map.insert(make_pair(file_descriptor, temp_character));
			}
			// delete(temp_character);
			write_mutex.unlock();
			continue;
		}
		// take garbage data outside of the socket buffer in the event a CHARACTER has died or is sending garbage data
		if(1 != read(file_descriptor, &type, 1)) {
			close(file_descriptor);
			if(our) {
				file_descriptor = disconnect_character(file_descriptor, &character_map);
				our->alive = 0;
				our->started = 0;
				update_character(our, &character_map);
			} else {
				disconnected_players--;
				file_descriptor = disconnected_players;
			}
			write_mutex.unlock();
			break;
		}
	}
}

int main(int argc, char **argv) {
	isaac.monster = 0;
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

	uint16_t lp = 5020;
	if(argc > 1) {
		lp = atoi(argv[1]);
		if(lp < 1) {
			printf("Invalid port entered: %s.\n", argv[1]);
			exit(1);
		}
		printf("grubserv listening on port %d.\n", lp);
	} else {
		printf("No port specified, grubserv listening on port %d.\n", lp);
	}

	struct sigaction sa;
	sa.sa_handler = handle_signal;
	sigaction(SIGINT, &sa, 0);
	sigaction(SIGPIPE, &sa, 0);

	struct sockaddr_in sad;
	sad.sin_port = htons(lp);
	sad.sin_addr.s_addr = INADDR_ANY;
	sad.sin_family = AF_INET;
	skt = socket(AF_INET, SOCK_STREAM, 0);

	if(skt == -1) {
		perror("socket");
		return 1;
	}
	if(bind(skt, (struct sockaddr *)(&sad), sizeof(struct sockaddr_in))) {
		perror("bind");
		return 1;
	}
	if(listen(skt, 5)) {
		perror("listen");
		return 1;
	}

	int cfd;
	struct sockaddr_in cad;
	socklen_t address_size = sizeof(struct sockaddr_in);

	while(1) {
		cfd = accept(skt, (struct sockaddr *)(&cad), &address_size);
		printf("Successful connection from %s.\n\n", inet_ntoa(cad.sin_addr));
		/*
		if(strcmp(inet_ntoa(cad.sin_addr), "127.0.0.1")) {
			printf("%s does not originate from isoptera, booting them from the server...\n", inet_ntoa(cad.sin_addr));
			class error* bad_cad = new error(0, 54, (char *)"Attempt to connect to grubserv from non-local address");
			bad_cad->send(cfd);
			close(cfd);
			delete(bad_cad);
			continue;
		}
		*/
		clients_mutex.lock();
		lurk_version->send(cfd);
		lurk_game->send(cfd);
		clients.push_back(new client(cfd, inet_ntoa(cad.sin_addr)));
		clients_mutex.unlock();

		cleanup_clients();
		printf("There are %lu current client(s).\n", clients.size());
	}

	return 0;
}

// C
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<bits/stdc++.h>
// C++
#include<cstring>
#include<mutex>
#include<thread>
#include<vector>
#include<map>
#include<string>
#include<algorithm>
// header files
#include "objects.h"
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

int skt;
vector<client*> clients;
mutex clients_mutex;
mutex server_mutex;

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
					strncpy(message_error->error_message, (char *)"Attempt to send MESSAGE to disconnected or nonexistent player", message_error->error_length);
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

	storeObjects();

	uint16_t lp = 5020;

	if(argc > 1) {
		lp = stoi(argv[1]);

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

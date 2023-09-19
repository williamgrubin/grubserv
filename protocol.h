#include <cstring>
#include <cmath>
#include <vector>
#include <map>
using namespace std;

class client {
	public:
		int file_descriptor;
		string address;
		mutex write_mutex;
		thread t;

		void run();
		static void start_own_thread(client* us) { us->run(); }
		client(int our_fd, string a) : file_descriptor(our_fd), t(start_own_thread, this), address(a) { }
		~client() { }
};

class fight {
	public:
		uint8_t type = 3;

		fight() { }
		bool receive(int cfd) {
			if(1 != read(cfd, this, 1)) {
				printf("Failed to receive() FIGHT from descriptor %d.\n", cfd);
				return false;
			}
			return true;
		}
}__attribute__((packed));

class pvp_fight {
public:
	uint8_t type = 4;
	char player_name[32];

	pvp_fight() { }
	bool receive(int cfd) {
		if(33 != recv(cfd, this, 33, MSG_WAITALL)) {
			printf("Failed to receive() PVP from descriptor %d.\n", cfd);
			return false;
		}
		return true;
	}
}__attribute__((packed));

class loot {
	public:
		uint8_t type = 5;
		char player_name[32];

		loot() { }
		bool receive(int cfd) {
			if(33 != recv(cfd, this, 33, MSG_WAITALL)) {
				printf("Failed to receive() LOOT from descriptor %d.\n", cfd);
				return false;
			}
			return true;
		}
}__attribute__((packed));

class error {
	public:
		uint8_t type = 7;
		uint8_t error_code;
		uint16_t error_length;
		char *error_message = 0;

		error() { }
		error(uint8_t ec, uint8_t el, char *em) : error_code(ec), error_length(el) {
			error_message = (char *)malloc(error_length);
			strncpy(error_message, em, error_length);
		}
		bool send(int cfd) {
			if(4 != write(cfd, this, 4)) { return false; }
			if(error_length != write(cfd, error_message, error_length)) { return false; }
			return true;
		}
		~error() { if(error_message) { free(error_message); } }
}__attribute__((packed));

class accept {
	public:
		uint8_t type = 8;
		uint8_t accepted_type;

		accept(uint8_t at) : accepted_type(at) { }
		bool send(int cfd) {
			if(2 != write(cfd, this, 2)) { return false; }
			return true;
		}
}__attribute__((packed));

class room {
	public:
		uint8_t type = 9;
		uint16_t room_number;
		char room_name[32];
		uint16_t description_length;
		char *room_description = 0;

		room(uint16_t rnum, char *rname, uint16_t dl, char *rdesc) : room_number(rnum), description_length(dl) {
			strncpy(room_name, rname, sizeof(room_name));
			room_description = (char *)malloc(description_length);
			strncpy(room_description, rdesc, description_length);
		}
		bool send(int cfd) {
			if(37 != write(cfd, this, 37)) { return false; }
			if(description_length != write(cfd, room_description, description_length)) { return false; }
			return true;
		}
		bool send_connection(int cfd) {
			type = 13;
			if(37 != write(cfd, this, 37)) { return false; }
			if(description_length != write(cfd, room_description, description_length)) { return false; }
			type = 9;
			return true;
		}
		~room() { if(room_description) { free(room_description); } }
}__attribute__((packed));

class character {
	public:
		uint8_t type = 10;
		char player_name[32];
		uint8_t reserved:3;
		uint8_t ready:1;
		uint8_t started:1;
		uint8_t monster:1;
		uint8_t join_battle:1;
		uint8_t alive:1;
		uint16_t attack_damage;
		uint16_t defense_rating;
		uint16_t regeneration;
		uint16_t health_points = 75;
		uint16_t gold_count = 50;
		uint16_t current_room = 0;
		uint16_t description_length;
		char *player_description = 0;

		character() { }
		character(char *pn, uint16_t ad, uint16_t dr, uint16_t rg, uint16_t hp,
				  uint16_t gc, uint16_t cr, uint16_t dl, char *pd) : 
				  reserved(0), ready(1), started(1), monster(1), join_battle(0), alive(1),
				  attack_damage(ad), defense_rating(dr), regeneration(rg), health_points(hp),
				  gold_count(gc), current_room(cr), description_length(dl) {
			strncpy(player_name, pn, sizeof(player_name));
			player_description = (char *)malloc(description_length);
			strncpy(player_description, pd, description_length);
		}

		bool receive(int cfd) {
			if(48 != recv(cfd, this, 48, MSG_WAITALL)) { return false; }
			reserved = (reserved == 0) ? reserved : 0;
			// players should always be ready
			ready = (ready == 1) ? ready : 1;
			// characters that have not started yet must be initialized to 0
			started = (started == 0) ? started : 0;
			// player-provided characters should never be monsters
			monster = (monster == 0) ? monster : 0;
			// characters should always be alive upon creation
			alive = (alive == 1)? alive : 1;
			health_points = (health_points == 75)? health_points : 75;
			gold_count = (gold_count == 50) ? gold_count : 50;
			current_room = (current_room == 0) ? current_room : 0;
			player_description = (char *)malloc(description_length);

			if(description_length != recv(cfd, player_description, description_length, MSG_WAITALL)) {
				printf("Failed to receive() CHARACTER description from descriptor %d.\n", cfd);
				return false;
			}

			return true;
		}

		bool send(int cfd) {
			if(48 != write(cfd, this, 48)) { return false; }
			if(description_length != write(cfd, player_description, description_length)) { return false; }
			return true;
		}
		
		~character() { if(player_description) { free(player_description); } }
}__attribute__((packed));

class message {
	public:
		uint8_t type = 1;
		uint16_t missive_length;
		char recipient_name[32];
		char sender_name[30];
		char narration_marker[2];
		char *missive = 0;

		message() { }
		message(uint16_t ml, char *rn, char *sn, bool nm, char *m) : missive_length(ml) {
			strncpy(recipient_name, rn, sizeof(recipient_name));
			strncpy(sender_name, sn, sizeof(sender_name));
			if(nm) {
				narration_marker[0] = 0;
				narration_marker[1] = 1;
			}
			missive = (char *)malloc(missive_length);
			strncpy(missive, m, missive_length);
			}
		bool receive(int cfd, map<int, character*> character_map) {
			if(67 != recv(cfd, this, 67, MSG_WAITALL)) {
				return false;
			}
			missive = (char *)malloc(missive_length);
			if(missive_length != recv(cfd, missive, missive_length, MSG_WAITALL)) {
				return false;
			}
			return true;
		}
		bool send(int cfd) {
			if(67 != write(cfd, this, 67))
				return false;
			if(missive_length != write(cfd, missive, missive_length))
				return false;
			return true;
		}
		~message() { if(missive) { free(missive); } }
}__attribute__((packed));

class changeroom {
	public:
		uint8_t type = 2;
		uint16_t room_number;

		changeroom() { }
		bool receive(int cfd) {
			if(3 != read(cfd, this, 3)) { return false; }
			return true;
		}
		bool send(int cfd, class room *source, class room *destination, class character *client_character, map<int, character*> character_map, vector<character*> npcs, vector<room*> rooms) {
			// send the desired room to ourselves
			if(!destination->send(cfd)) {
				printf("CHANGEROOM destination send() to descriptor %d failed.\n", cfd);
				return false;
			}
			for(auto& i : character_map) {
				if(i.second) {
					if(i.second->current_room == client_character->current_room && strcmp(i.second->player_name, client_character->player_name)) {
						// write neighboring players to ourselves
						if(!i.second->send(cfd)) {
							printf("CHANGEROOM neighbor send() to descriptor %d failed.\n", cfd);
							return false;
						}
						// if the other player in the room is still connected
						if(i.first > 0) {
							// then write ourselves to them
							if(!client_character->send(i.first)) {
								printf("CHANGEROOM client_character send() to descriptor %d failed.\n", i.first);
								return false;
							}
						}
					}
				}
			}
			for(auto& i : npcs) {
				if(i->current_room == client_character->current_room) {
					// write neighboring NPCs to ourselves
					if(!i->send(cfd)) {
						printf("CHANGEROOM NPC send() to descriptor %d failed.\n", cfd);
						return false;
					}
				}
			}
			// send the connecting rooms to ourselves
			for(auto& i : rooms) {
				if(i->room_number == (client_character->current_room + 1) || i->room_number == (client_character->current_room - 1)) {
					if(!i->send_connection(cfd)) {
						printf("CHANGEROOM send_connection() to descriptor %d failed.\n", cfd);
						return false;
					}
				}
			}
			// send our updated character to ourselves
			if(!client_character->send(cfd)) {
				printf("CHANGEROOM client_character send() to descriptor %d failed.\n", cfd);
				return false;
			}
			return true;
		}
}__attribute__((packed));

class start {
	public:
		uint8_t type = 6;

		bool send(int cfd, class room *start_room, class character *client_character, map<int, character*> character_map, vector<character*> npcs) {
			// send the reprised/created character to the client
			if(!client_character->send(cfd)) {
				printf("START client_character send() to cfd failed.\n");
				return false;
			}
			// send the room we're starting in
			if(!start_room->send(cfd)) {
				printf("START start_room send() to descriptor %d failed.\n", cfd);
				return false;
			}
			for(auto& i : character_map) {
				if(i.second) {
					if(i.second->current_room == client_character->current_room) {
						// send neighboring CHARACTERs to ourselves
						if(!i.second->send(cfd)) {
							printf("START neighbor send() to descriptor %d failed.\n", cfd);
							return false;
						}
						// send our new CHARACTER to neighboring online players
						if(i.first > 0) {
							if(!client_character->send(i.first)) {
								printf("START client_character send() to descriptor %d failed.\n", i.first);
								return false;
							}
						}
					}
				}
			}
			for(auto& i : npcs) {
				if(i->current_room == client_character->current_room && strcmp(i->player_name, client_character->player_name)) {
					// send neighboring NPCs to ourselves
					if(!i->send(cfd)) {
						printf("START NPC send() to descriptor %d failed.\n", cfd);
						return false;
					}
				}
			}
			return true;
		}
}__attribute__((packed));

class game {
	public:
		uint8_t type = 11;
		uint16_t initial_points;
		uint16_t stat_limit;
		uint16_t description_length;
		char *description = 0;

		game(uint16_t ip, uint16_t sl, uint16_t dl, char *d) : initial_points(ip), stat_limit(sl), description_length(dl) {
			description = (char *)malloc(description_length);
			strncpy(description, d, description_length);
		}
		bool send(int cfd) {
			if(7 != write(cfd, this, 7))
				return false;
			if(description_length != write(cfd, description, description_length))
				return false;
			return true;
		}
		~game() { if(description) free(description); }
}__attribute__((packed));

class version {
	public:
		uint8_t type = 14;
		uint8_t major;
		uint8_t minor;
		uint16_t extension_length = 0;

		version(uint8_t maj, uint8_t min) : major(maj), minor(min) { }
		bool send(int cfd) {
			return 5 == write(cfd, this, 5);
		}
}__attribute__((packed));

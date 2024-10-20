/*
 *  Předmět: Algoritmy (IAL) - FIT VUT v Brně
 *  Rozšíření pro příklad c206.c (Dvousměrně vázaný lineární seznam)
 *  Vytvořil: Daniel Dolejška, září 2024
 */

#include "c206-ext.h"

bool error_flag;
bool solved;

/**
 * Tato metoda simuluje příjem síťových paketů s určenou úrovní priority.
 * Přijaté pakety jsou zařazeny do odpovídajících front dle jejich priorit.
 * "Fronty" jsou v tomto cvičení reprezentovány dvousměrně vázanými seznamy
 * - ty totiž umožňují snazší úpravy pro již zařazené položky.
 * 
 * Parametr `packetLists` obsahuje jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Pokud fronta s odpovídající prioritou neexistuje, tato metoda ji alokuje
 * a inicializuje. Za jejich korektní uvolnení odpovídá volající.
 * 
 * V případě, že by po zařazení paketu do seznamu počet prvků v cílovém seznamu
 * překročil stanovený MAX_PACKET_COUNT, dojde nejdříve k promazání položek seznamu.
 * V takovémto případě bude každá druhá položka ze seznamu zahozena nehledě
 * na její vlastní prioritu ovšem v pořadí přijetí.
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param packet Ukazatel na strukturu přijatého paketu
 */
void receive_packet( DLList *packetLists, PacketPtr packet ) {
	if (packetLists == NULL || packet == NULL) {
		error_flag = true;
		return;
	}
	DLL_First(packetLists);
	char priority_order = 0;
	while(DLL_IsActive(packetLists)){ // Goes thrue all elements in packetLists
		QosPacketListPtr currentqos = NULL;
		DLL_GetValue(packetLists, (long *)&currentqos); // Gets value of current element and converts it to QosPacketListPtr
		if(currentqos->priority == packet->priority) { // If the priority matches the packet it is inserted to the list
			if(currentqos->list->currentLength + 1 > MAX_PACKET_COUNT) { // If the list is full, every second element is deleted
				DLL_First(currentqos->list);
				while(DLL_IsActive(currentqos->list)) {
					DLL_DeleteAfter(currentqos->list);
					DLL_Next(currentqos->list);
				}
			}
			DLL_InsertLast(currentqos->list, (long)packet);
			return;
		} else if(currentqos->priority > packet->priority) { // Because I am ordering the lists by priority, if current priority is higher than the packet, it means that the QosPacketList doesnt exist and the packet should be inserted before the current list
			priority_order = 1;
			break;
		}
		DLL_Next(packetLists); // Goes to next element in packetLists
	}
	QosPacketListPtr newqos = (QosPacketListPtr)malloc(sizeof(QosPacketList));
	if(newqos == NULL) {
		error_flag = true;
		return;
	}
	newqos->priority = packet->priority;
	newqos->list = (DLList *)malloc(sizeof(DLList));
	if(newqos->list == NULL) {
		free(newqos);
		error_flag = true;
		return;
	}
	DLL_Init(newqos->list);
	DLL_InsertLast(newqos->list, (long)packet);
	if(priority_order) { // Gets inserted before the current list
		DLL_InsertBefore(packetLists, (long)newqos);
	} else { // If the priority highest or the list is empty, it gets inserted last
		DLL_InsertLast(packetLists, (long)newqos);
	}
}

/**
 * Tato metoda simuluje výběr síťových paketů k odeslání. Výběr respektuje
 * relativní priority paketů mezi sebou, kde pakety s nejvyšší prioritou
 * jsou vždy odeslány nejdříve. Odesílání dále respektuje pořadí, ve kterém
 * byly pakety přijaty metodou `receive_packet`.
 * 
 * Odeslané pakety jsou ze zdrojového seznamu při odeslání odstraněny.
 * 
 * Parametr `packetLists` obsahuje ukazatele na jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Parametr `outputPacketList` obsahuje ukazatele na odeslané pakety (`PacketPtr`).
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param outputPacketList Ukazatel na seznam paketů k odeslání
 * @param maxPacketCount Maximální počet paketů k odeslání
 */
void send_packets( DLList *packetLists, DLList *outputPacketList, int maxPacketCount ) {
	if (packetLists == NULL || outputPacketList == NULL || maxPacketCount <= 0){
		error_flag = true;
		return;
	}
	DLL_Last(packetLists); // Stars at the end because it's ordered from lowest priority to the highest
	int sent_packets = 0; // Counter for sent packets
	while (sent_packets < maxPacketCount && DLL_IsActive(packetLists)) { // Goes thrue all elements in packetLists while there are packets to send
		QosPacketListPtr current = (QosPacketListPtr)packetLists->activeElement->data; // Gets the current QosPacketList
		DLL_First(current->list);
		while (sent_packets < maxPacketCount && DLL_IsActive(current->list)) { // Goes thrue all elements in the current list while there are packets to send
			long packet = current->list->activeElement->data;
			DLL_InsertLast(outputPacketList, packet); // Inserts the packet to the outputPacketList
			sent_packets++;
			DLL_Next(current->list); // Moves to the next packet in the list
			DLL_DeleteFirst(current->list); // Deletes sent packet from the current list
		}
		DLL_Previous(packetLists); // Moves to the previous QosPacketList
	}
}

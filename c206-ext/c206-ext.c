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
	while(DLL_IsActive(packetLists)){
		QosPacketListPtr currentqos = NULL;
		DLL_GetValue(packetLists, (long *)&currentqos);
		if(currentqos->priority == packet->priority) {
			if(currentqos->list->currentLength + 1 > MAX_PACKET_COUNT) {
				DLL_First(currentqos->list);
				while(DLL_IsActive(currentqos->list)) {
					DLL_DeleteAfter(currentqos->list);
					DLL_Next(currentqos->list);
				}
			}
			DLL_InsertLast(currentqos->list, (long)packet);
			return;
		} else if(currentqos->priority > packet->priority) {
			priority_order = 1;
			break;
		}
		DLL_Next(packetLists);
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
	if(priority_order) {
		DLL_InsertBefore(packetLists, (long)newqos);
	} else {
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
	DLL_Last(packetLists);
	int sent = 0;
	while (sent < maxPacketCount && DLL_IsActive(packetLists)) {
		QosPacketListPtr current = (QosPacketListPtr)packetLists->activeElement->data;
		DLL_First(current->list);
		while (sent < maxPacketCount && DLL_IsActive(current->list)) {
			long packet = current->list->activeElement->data;
			DLL_InsertLast(outputPacketList, packet);
			sent++;
			DLL_Next(current->list);
			DLL_DeleteFirst(current->list);
		}
		DLL_Previous(packetLists);
	}
}

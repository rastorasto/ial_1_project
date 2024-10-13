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
	DLL_First(packetLists);
	QosPacketListPtr tmp = NULL;
	while(packetLists->activeElement != NULL){
		QosPacketListPtr tmp = (QosPacketListPtr)packetLists->activeElement->data;
		if(tmp->priority == packet->priority){
			if((tmp->list->currentLength)+1 > MAX_PACKET_COUNT){
				DLL_First(tmp->list);
				while(tmp->list->activeElement != NULL){
					DLL_DeleteAfter(tmp->list);
					DLL_Next(tmp->list);
				}
			}
			DLL_InsertLast(tmp->list, (long)packet);
			return;
		} else if (tmp->priority > packet->priority){
			break;
		}
		DLL_Next(packetLists);
	}
	QosPacketListPtr newQosPacketList = (QosPacketListPtr)malloc(sizeof(QosPacketList));
	if (newQosPacketList == NULL) {
		error_flag = true;
		return;
	}
	newQosPacketList->priority = packet->priority;
	newQosPacketList->list = (DLList *)malloc(sizeof(DLList));
	if (newQosPacketList->list == NULL) {
		free(newQosPacketList);
		error_flag = true;
		return;
	}
	DLL_Init(newQosPacketList->list);
	if(packetLists->activeElement == NULL){
		DLL_InsertLast(packetLists, (long)newQosPacketList);
	} else {
		if(tmp->priority > packet->priority){
			DLL_InsertBefore(packetLists, (long)newQosPacketList);
		} else {
			DLL_InsertAfter(packetLists, (long)newQosPacketList);
		}
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
	// QosPacketListPtr tmp = (QosPacketListPtr)packetLists->activeElement->data;
	// DLLElementPtr tmpElement = packetLists->activeElement;
	// while(tmpElement != packetLists->lastElement){
	// 	DLL_First(packetLists);
	// 	while(packetLists->activeElement != packetLists->lastElement){
	// 		if (tmp->priority < ((QosPacketListPtr)packetLists->activeElement->data)->priority && (QosPacketListPtr)packetLists->currentLength > 0){
	// 			tmp = (QosPacketListPtr)packetLists->activeElement->data;
	// 		}
	// 		DLL_Next(packetLists);
	// 	}
	// 	DLL_First(tmp->list);
	// 	while(tmp->list->activeElement != tmp->list->lastElement){
	// 		if(maxPacketCount > tmp->list->currentLength){
	// 			return;
	// 		}
	// 		DLLElementPtr send = (DLLElementPtr)malloc(sizeof(struct DLLElement));
	// 		DLL_GetValue(tmp->list, (long *)send);
	// 		DLL_InsertLast(outputPacketList, (long)send);
	// 		DLL_DeleteFirst(tmp->list);
	// 		DLL_Next(tmp->list);
	// 	}
	// }
}

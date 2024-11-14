#include "xil_types.h" // include tipuri predefinite: u8
#include "xparameters.h" // include constante ce descriu sistemul
#define TX_ADDR (XPAR_UARTLITE_0_BASEADDR + 0x04)
#define UART_STATUS (XPAR_UARTLITE_0_BASEADDR + 0x08)
#include <stdlib.h>

typedef struct _Mailbox
{
	volatile u32* mbox_baseaddr;

} *Mailbox;

int MboxIsEmpty(Mailbox mbox){
	volatile u32* MboxIfBaseAddr = mbox->mbox_baseaddr;
	return (*(volatile u32 *)((volatile u8*)MboxIfBaseAddr + 0x10)) & 1;
}

/// There are 2 physical mailbox modules
/// 'physical_mailbox' should be 0 or 1
Mailbox MakeMailbox(int physical_mailbox)
{
	volatile Mailbox mbox = malloc(sizeof(Mailbox));

	// assign the address based on selected Mailbox.
	if (physical_mailbox == 0)
		mbox->mbox_baseaddr = 0x43610000;
	else mbox->mbox_baseaddr = 0x43620000;

	return mbox;
}

/// Send data to the UART peripheral. If connected via Putty etc.,
/// this acts like a print function. (e.g. printf)
void UartSend(char *data, int len)
{
	int i = 0;
	while(i < len)
	{
		if(!(*(volatile u32 *)(UART_STATUS)&(1<<3)))
		{
			*(volatile u8 *)(TX_ADDR) = data[i];
			i++;
		}
	}
}



/// <summary>
/// Citeste un sir de lungime dataLen din coada de primire a datelor si stocheaza datele primite incepand cu adresa destDataPtr
/// Functia este blocanta. Va astepta pana se va citi numarul de intregi dorit
/// </summary>
void MboxReadBlocking(Mailbox mbox, int* destDataPtr, int dataLen) {
	int i = 0;
	volatile u32* MboxIfBaseAddr = mbox->mbox_baseaddr;
	while(i < dataLen)
	{
		while(MboxIsEmpty(mbox))
		{
			//buffer gol
		}
		destDataPtr[i] = *(volatile int *)(MboxIfBaseAddr + +0x8);
		i++;
	}
}

int main()
{
	char buff_c1[32] = "Initial msg.\n";
	MboxReadBlocking(XPAR_MAILBOX_0_IF_1_BASEADDR, (int*)buff_c1, 12/4);
	Mailbox mbox = MakeMailbox(0);
	buff_c1[0] = MboxIsEmpty(mbox) + '0';
	UartSend(buff_c1, 13);
	return 0;
}


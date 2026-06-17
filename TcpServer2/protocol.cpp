#include "protocol.h"

PDU *mkPDU(uint uiMsgLen)
{
    //计算需要分配的空间大小
    uint uiPDULen=sizeof(PDU)+uiMsgLen;
    PDU *pdu=(PDU*)malloc(uiPDULen);
    //判断是否为空
    if(NULL==pdu)
    {
        exit(EXIT_FAILURE);
    }
    //初始化
    memset(pdu,0,uiPDULen);
    pdu->uiPDULen=uiPDULen;
    pdu->uiMsgLen=uiMsgLen;
    return pdu;
}

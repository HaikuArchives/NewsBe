ServerGroupTree::AddGroup(const char *sGroup)
{

	pTempItem = myTopItem;
	pBuffer = strdup(sGroup);
	pTempValue = strtok(pBuffer, ".");
	while(pTempValue != NULL) {
		switch(strcmp(pTemp, pTempItem->Value)) 
		{
			case -1:
				if(pTempItem->NextSibling == NULL)  // no more sibling, but stull some tokens
				{
					pTempItem = AddNewSibling(pTempItem, pTempValue);
					pTempValue = strtok(NULL, ".");
				}
				else
				{
					pTempItem = pTempItem->NextSibling;
				}				 
				break;
			case 0:
				pTempItem = pTempItem->Child;
				pTempValue = strtok(NULL, ".");
				break;
			case 1:
				pTempItem = AddNewSibling(pTempItem, pTempValue);
				pTempValue = strtok(NULL, ".");
				break;
		}
}

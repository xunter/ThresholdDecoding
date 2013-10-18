#include "StdAfx.h"
#include "ModelingResultConsoleItemStorage.h"
#include "ByteUtil.h"

namespace ThresholdDecoding {
	void ModelingResultConsoleItemStorage::Complete() {
		//nothing
	};

	void ModelingResultConsoleItemStorage::Store(ModelingResultItem *item) {
		printf("ModelingResultConsoleItem:\n");
		printf("Source Data: \n");
		BinaryData *sourceData = item->GetSourceBlock();
		ByteUtil::ShowDataBlockOnConsole(sourceData->GetData(), sourceData->GetBitsCount());
		
		printf("Encoded Data: \n");
		BinaryData *encodedData = item->GetEncodedBlock();
		ByteUtil::ShowDataBlockOnConsole(encodedData->GetData(), encodedData->GetBitsCount());
		
		printf("Received Data: \n");
		BinaryData *receivedData = item->GetReceivedBlock();
		ByteUtil::ShowDataBlockOnConsole(receivedData->GetData(), receivedData->GetBitsCount());
		
		printf("Decoded Data: \n");
		BinaryData *decodedData = item->GetDecodedBlock();
		ByteUtil::ShowDataBlockOnConsole(decodedData->GetData(), decodedData->GetBitsCount());

		printf("Does Source equal Decoded: %d",  item->IsResultEqualsOriginal() ? 1 : 0);
		printf("\n");
	};
}
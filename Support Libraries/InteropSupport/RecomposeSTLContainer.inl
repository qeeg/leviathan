#include "ISTLObjectKeyMarshaller.h"
#include "IMarshallingObject.h"
#include "MarshalObject.h"
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
#include <type_traits>
#include <memory.h>
#include <utility>
#endif
namespace InteropSupport {

#ifdef _MSC_VER
#pragma warning(push)
//Disable warning about our use of std::is_pod causing a conditional expression to be constant. The
//code behaves as intended, and the compiler is free to optimize away the dead branch.
#pragma warning(disable:4127)
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::vector<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
	if(std::is_pod<ElementType>::value)
	{
		const ElementType* itemArrayWithType = (const ElementType*)itemArray;
		element.assign(itemArrayWithType + elementArrayIndex, itemArrayWithType + elementArrayIndex + containerEntryCount);
		elementArrayIndex += containerEntryCount;
	}
	else
#endif
	{
		element.resize(containerEntryCount);
		for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
		{
			RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, element[containerEntryNo]);
		}
	}
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, size_t ArraySize>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::array<ElementType, ArraySize>& element)
{
	if(std::is_pod<ElementType>::value)
	{
		const ElementType* itemArrayWithType = (const ElementType*)itemArray;
		memcpy((void*)element.data(), (const void*)&itemArrayWithType[elementArrayIndex], ArraySize * sizeof(ElementType));
		elementArrayIndex += ArraySize;
	}
	else
	{
		for(size_t containerEntryNo = 0; containerEntryNo < ArraySize; ++containerEntryNo)
		{
			RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, element[containerEntryNo]);
		}
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::list<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	std::list<ElementType>::iterator containerIterator = element.begin();
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerIterator;
	}
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::forward_list<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	std::forward_list<ElementType>::iterator containerIterator = element.begin();
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerIterator;
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::deque<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	element.resize(containerEntryCount);
	std::deque<ElementType>::iterator containerIterator = element.begin();
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, *containerIterator);
		++containerIterator;
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::set<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		ElementType containerEntry;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
		element.insert(std::move(containerEntry));
#else
		element.insert(containerEntry);
#endif
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multiset<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		ElementType containerEntry;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
		element.insert(std::move(containerEntry));
#else
		element.insert(containerEntry);
#endif
	}
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_set<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		ElementType containerEntry;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		element.insert(std::move(containerEntry));
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multiset<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		ElementType containerEntry;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		element.insert(std::move(containerEntry));
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::map<KeyType, ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		ElementType containerEntry;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
		element.insert(std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), std::move(containerEntry)));
#else
		element.insert(std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), containerEntry));
#endif
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::multimap<KeyType, ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		ElementType containerEntry;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
		element.insert(std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), std::move(containerEntry)));
#else
		element.insert(std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), containerEntry));
#endif
	}
}

#ifdef INTEROPSUPPORT_CPP11SUPPORTED
//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_map<KeyType, ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		ElementType containerEntry;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
		element.insert(std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), std::move(containerEntry)));
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType, class KeyType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::unordered_multimap<KeyType, ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		ElementType containerEntry;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, containerEntry);
		ISTLObjectKeyMarshaller<KeyType>* keyWrapper = (ISTLObjectKeyMarshaller<KeyType>*)keyMarshallerArray[keyMarshallerArrayIndex];
		element.insert(std::pair<KeyType, ElementType>(keyWrapper->RemoveKey(), std::move(containerEntry)));
	}
}
#endif

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::stack<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		ElementType entry;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, entry);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
		element.push(std::move(entry));
#else
		element.push(entry);
#endif
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::queue<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		ElementType entry;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, entry);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
		element.push(std::move(entry));
#else
		element.push(entry);
#endif
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::priority_queue<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	for(size_t containerEntryNo = 0; containerEntryNo < containerEntryCount; ++containerEntryNo)
	{
		ElementType entry;
		RecomposeSTLContainer(itemArray, elementSizeArray, keyMarshallerArray, elementArrayIndex, elementSizeArrayIndex, keyMarshallerArrayIndex+1, entry);
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
		element.push(std::move(entry));
#else
		element.push(entry);
#endif
	}
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, std::basic_string<ElementType>& element)
{
	size_t containerEntryCount = elementSizeArray[elementSizeArrayIndex++];
	const ElementType* itemArrayWithType = (const ElementType*)itemArray;
	element.assign(itemArrayWithType + elementArrayIndex, containerEntryCount);
	elementArrayIndex += containerEntryCount;
}

//----------------------------------------------------------------------------------------
template<class ElementType>
void RecomposeSTLContainer(const void* itemArray, const size_t elementSizeArray[], ISTLObjectKeyMarshallerBase* const keyMarshallerArray[], size_t& elementArrayIndex, size_t& elementSizeArrayIndex, size_t keyMarshallerArrayIndex, ElementType& element)
{
	const ElementType* itemArrayWithType = (const ElementType*)itemArray;
	const ElementType& arrayElement = itemArrayWithType[elementArrayIndex++];
	if(!MarshalObject<INTEROPSUPPORT_ISBASEOF(IMarshallingObject, ElementType)>(&arrayElement, &element))
	{
#ifdef INTEROPSUPPORT_CPP11SUPPORTED
		element = std::move(arrayElement);
#else
		element = arrayElement;
#endif
	}
}

//Restore the disabled warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif

} //Close namespace InteropSupport
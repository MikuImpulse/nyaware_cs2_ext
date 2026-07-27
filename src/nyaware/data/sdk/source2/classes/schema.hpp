#pragma once

#include <cstdint>

#include "utils/memory.hpp"

class CSchemaField {
public:
	const char* m_szName;
	void* m_pType;
	uint32_t m_uOffset;
	uint32_t m_uMetadataSize;
	void* m_pMetadata;
};

struct SchemaClassInfoData_t {
	char pad[0x8];
	const char* m_szName;
	const char* m_szProjectName;
	const char* m_szCPPName;
	int m_nSize;
	int16_t m_nFieldCount;
	int16_t m_nStaticMetadataCount;
	uint8_t m_uAlignment;
	uint8_t m_uBaseClassCount;
	int16_t m_nMultipleInheritanceDepth;
	int16_t m_nSingleInheritanceDepth;

	char pad2[0x2];
	CSchemaField* m_pFields;

	char pad3[0x38];
};

class CSchemaDeclaredClass {
public:
	char pad[0x8];
	const char* m_szName;
	const char* m_szModuleName;
	const char* m_szUnknownStr;
	SchemaClassInfoData_t* m_pClassInfoData;
};

class CSchemaDeclaredClassEntry {
public:
	uint64_t m_nHash[2];
	CSchemaDeclaredClass* m_pDeclaredClass;
};

class CSchemaSystemTypeScope {
public:
	char pad[0x8];
	char m_szName[256];

	char pad2[0x368];
	uint16_t m_uNumDeclaredClasses;

	char pad3[0x6];
	CSchemaDeclaredClassEntry* m_pDeclaredClasses;
};

class CSchemaSystem {
public:
	char pad[0x190];
	int m_nScopeSize;

	char pad2[0x4];
	CSchemaSystemTypeScope** m_pScopeArray;
};
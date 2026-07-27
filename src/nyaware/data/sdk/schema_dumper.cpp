#include "schema_dumper.hpp"

#include "sdk.hpp"

bool c_schema_dumper::dump(uintptr_t schema_system_interface) {
    if (!schema_system_interface) return false;

    uintptr_t scope_array_ptr = mem.read<uintptr_t>(schema_system_interface + offsetof(CSchemaSystem, m_pScopeArray));
    int scopes_count = mem.read<int>(schema_system_interface + offsetof(CSchemaSystem, m_nScopeSize));

    void** dpScopeArray = new void*[scopes_count];
    if (!mem.read(scope_array_ptr, dpScopeArray, scopes_count * sizeof(void*))) {
        return false;
    }

    for (int i = 0; i < scopes_count; i++) {
        CSchemaSystemTypeScope schema_scope = mem.read<CSchemaSystemTypeScope>((uintptr_t)dpScopeArray[i]);
        if (!schema_scope.m_pDeclaredClasses) continue;

        CSchemaDeclaredClassEntry* declared_class_entries = new CSchemaDeclaredClassEntry[schema_scope.m_uNumDeclaredClasses + 1U];
        if (!mem.read((uintptr_t)schema_scope.m_pDeclaredClasses, declared_class_entries, (schema_scope.m_uNumDeclaredClasses + 1U) * sizeof(CSchemaDeclaredClassEntry))) {
            continue;
        }

        if (strcmp(schema_scope.m_szName, "client.dll") != 0) continue;

        for (uint16_t j = 0U; j < schema_scope.m_uNumDeclaredClasses; j++) {
            CSchemaDeclaredClass declared_class = mem.read<CSchemaDeclaredClass>((uintptr_t)declared_class_entries[j].m_pDeclaredClass);
            SchemaClassInfoData_t schema_class = mem.read<SchemaClassInfoData_t>((uintptr_t)declared_class.m_pClassInfoData);

            std::string class_name = mem.read_str((uintptr_t)declared_class.m_szName);

            for (int16_t k = 0; k < schema_class.m_nFieldCount; k++) {
                CSchemaField schema_field = mem.read<CSchemaField>((uintptr_t)schema_class.m_pFields + sizeof(CSchemaField) * k);
                if (!schema_field.m_pType) continue;

                std::string field_name = mem.read_str((uintptr_t)schema_field.m_szName);
                schema_offset[class_name][field_name] = schema_field.m_uOffset;
            }
        }
    }

    delete[] dpScopeArray;
    return true;
}
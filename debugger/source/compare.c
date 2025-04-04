#include "../include/compare.h"
#define FUZZY_TOLERANCE 1.0

int compare_value_exact(BYTE *pScanValue, BYTE *pMemoryValue, size_t valTypeLength) {
    return memcmp(pScanValue, pMemoryValue, valTypeLength) == 0;
}

// int compare_value_exact(BYTE *pScanValue, BYTE *pMemoryValue, size_t valTypeLength) {
//     int isFound = FALSE;
//     for (size_t j = 0; j < valTypeLength - 1; j++) {
//         isFound = (pScanValue[j] == pMemoryValue[j]);
//         if (!isFound)
//             break;
//     }
// 
//     return isFound;
// }

int compare_value_fuzzy(enum cmd_proc_scan_valuetype valType, BYTE *pScanValue, BYTE *pMemoryValue) {
    if (valType == SCAN_TYPE_FLOAT) {
        float diff = *(float *)pScanValue - *(float *)pMemoryValue;
        return diff < FUZZY_TOLERANCE && diff > -FUZZY_TOLERANCE;
    }
    else if (valType == SCAN_TYPE_DOUBLE) {
        double diff = *(double *)pScanValue - *(double *)pMemoryValue;
        return diff < FUZZY_TOLERANCE && diff > -FUZZY_TOLERANCE;
    }

    return FALSE;
}

int compare_value_bigger_than(enum cmd_proc_scan_valuetype valType, BYTE *pScanValue, BYTE *pMemoryValue) {
    switch (valType) {
        case SCAN_TYPE_U8:      return *pMemoryValue > *pScanValue;
        case SCAN_TYPE_I8:       return *(int8_t *)pMemoryValue > *(int8_t *)pScanValue;
        case SCAN_TYPE_U16:     return *(uint16_t *)pMemoryValue > *(uint16_t *)pScanValue;
        case SCAN_TYPE_I16:      return *(int16_t *)pMemoryValue > *(int16_t *)pScanValue;
        case SCAN_TYPE_U32:     return *(uint32_t *)pMemoryValue > *(uint32_t *)pScanValue;
        case SCAN_TYPE_I32:      return *(int32_t *)pMemoryValue > *(int32_t *)pScanValue;
        case SCAN_TYPE_U64:     return *(uint64_t *)pMemoryValue > *(uint64_t *)pScanValue;
        case SCAN_TYPE_I64:      return *(int64_t *)pMemoryValue > *(int64_t *)pScanValue;
        case SCAN_TYPE_FLOAT:      return *(float *)pMemoryValue > *(float *)pScanValue;
        case SCAN_TYPE_DOUBLE:     return *(double *)pMemoryValue > *(double *)pScanValue;
        case SCAN_TYPE_BYTE_ARRAY:
        case SCAN_TYPE_STRING:
        default:
            return FALSE;
    };
}

int compare_value_smaller_than(enum cmd_proc_scan_valuetype valType, BYTE *pScanValue, BYTE *pMemoryValue) {
    switch (valType) {
        case SCAN_TYPE_U8:      return *pMemoryValue < *pScanValue;
        case SCAN_TYPE_I8:       return *(int8_t *)pMemoryValue < *(int8_t *)pScanValue;
        case SCAN_TYPE_U16:     return *(uint16_t *)pMemoryValue < *(uint16_t *)pScanValue;
        case SCAN_TYPE_I16:      return *(int16_t *)pMemoryValue < *(int16_t *)pScanValue;
        case SCAN_TYPE_U32:     return *(uint32_t *)pMemoryValue < *(uint32_t *)pScanValue;
        case SCAN_TYPE_I32:      return *(int32_t *)pMemoryValue < *(int32_t *)pScanValue;
        case SCAN_TYPE_U64:     return *(uint64_t *)pMemoryValue < *(uint64_t *)pScanValue;
        case SCAN_TYPE_I64:      return *(int64_t *)pMemoryValue < *(int64_t *)pScanValue;
        case SCAN_TYPE_FLOAT:      return *(float *)pMemoryValue < *(float *)pScanValue;
        case SCAN_TYPE_DOUBLE:     return *(double *)pMemoryValue < *(double *)pScanValue;
        case SCAN_TYPE_BYTE_ARRAY:
        case SCAN_TYPE_STRING:
        default:
            return FALSE;
    };
}

// Doesn't include cstom variable type for casting
#define COMPARE_BETWEEN_HELPER_REGULAR(ScanValue, MemValue, ExtraValue) \
    if (*ExtraValue > *ScanValue)                                       \
        return *MemValue > *ScanValue && *MemValue < *ExtraValue;       \
    else                                                                \
        return *MemValue < *ScanValue && *MemValue > *ExtraValue;  

// Supports a custom provided variable type to use for casting
#define COMPARE_BETWEEN_HELPER_CAST(type, ScanValue, MemValue, ExtraValue)                            \
    if (*((type*)ExtraValue) > *((type*)ScanValue))                                                   \
        return *((type*)MemValue) > *((type*)ScanValue) && *((type*)MemValue) < *((type*)ExtraValue); \
    else                                                                                              \
        return *((type*)MemValue) < *((type*)ScanValue) && *((type*)MemValue) > *((type*)ExtraValue);


int compare_value_between(enum cmd_proc_scan_valuetype valType, BYTE *pScanValue, BYTE *pMemoryValue, BYTE *pExtraValue) {
    switch (valType) {
        case SCAN_TYPE_U8:         COMPARE_BETWEEN_HELPER_REGULAR(pScanValue, pMemoryValue, pExtraValue);
        case SCAN_TYPE_I8:         COMPARE_BETWEEN_HELPER_CAST(int8_t, pScanValue, pMemoryValue, pExtraValue);
        case SCAN_TYPE_U16:        COMPARE_BETWEEN_HELPER_CAST(uint16_t, pScanValue, pMemoryValue, pExtraValue);
        case SCAN_TYPE_I16:        COMPARE_BETWEEN_HELPER_CAST(int16_t, pScanValue, pMemoryValue, pExtraValue);
        case SCAN_TYPE_U32:        COMPARE_BETWEEN_HELPER_CAST(uint32_t, pScanValue, pMemoryValue, pExtraValue);
        case SCAN_TYPE_I32:        COMPARE_BETWEEN_HELPER_CAST(int32_t, pScanValue, pMemoryValue, pExtraValue);
        case SCAN_TYPE_U64:        COMPARE_BETWEEN_HELPER_CAST(uint64_t, pScanValue, pMemoryValue, pExtraValue);
        case SCAN_TYPE_I64:        COMPARE_BETWEEN_HELPER_CAST(int64_t, pScanValue, pMemoryValue, pExtraValue);
        case SCAN_TYPE_FLOAT:      COMPARE_BETWEEN_HELPER_CAST(float, pScanValue, pMemoryValue, pExtraValue);
        case SCAN_TYPE_DOUBLE:     COMPARE_BETWEEN_HELPER_CAST(double, pScanValue, pMemoryValue, pExtraValue);
        case SCAN_TYPE_BYTE_ARRAY:
        case SCAN_TYPE_STRING:
        default:
            return FALSE;
    };
}

int compare_value_increased(enum cmd_proc_scan_valuetype valType, BYTE *pScanValue, BYTE *pMemoryValue, BYTE *pExtraValue) {
    switch (valType) {
        case SCAN_TYPE_U8:      return *pMemoryValue > *pExtraValue;
        case SCAN_TYPE_I8:       return *(int8_t *)pMemoryValue > *(int8_t *)pExtraValue;
        case SCAN_TYPE_U16:     return *(uint16_t *)pMemoryValue > *(uint16_t *)pExtraValue;
        case SCAN_TYPE_I16:      return *(int16_t *)pMemoryValue > *(int16_t *)pExtraValue;
        case SCAN_TYPE_U32:     return *(uint32_t *)pMemoryValue > *(uint32_t *)pExtraValue;
        case SCAN_TYPE_I32:      return *(int32_t *)pMemoryValue > *(int32_t *)pExtraValue;
        case SCAN_TYPE_U64:     return *(uint64_t *)pMemoryValue > *(uint64_t *)pExtraValue;
        case SCAN_TYPE_I64:      return *(int64_t *)pMemoryValue > *(int64_t *)pExtraValue;
        case SCAN_TYPE_FLOAT:      return *(float *)pMemoryValue > *(float *)pExtraValue;
        case SCAN_TYPE_DOUBLE:     return *(double *)pMemoryValue > *(double *)pExtraValue;
        case SCAN_TYPE_BYTE_ARRAY:
        case SCAN_TYPE_STRING:
        default:
            return FALSE;
    };
}

int compare_value_increased_by(enum cmd_proc_scan_valuetype valType, BYTE *pScanValue, BYTE *pMemoryValue, BYTE *pExtraValue) {
    switch (valType) {
        case SCAN_TYPE_U8:      return *pMemoryValue == (*pExtraValue + *pScanValue);
        case SCAN_TYPE_I8:       return *(int8_t *)pMemoryValue == (*(int8_t *)pExtraValue + *(int8_t *)pScanValue);
        case SCAN_TYPE_U16:     return *(uint16_t *)pMemoryValue == (*(uint16_t *)pExtraValue + *(uint16_t *)pScanValue);
        case SCAN_TYPE_I16:      return *(int16_t *)pMemoryValue == (*(int16_t *)pExtraValue + *(int16_t *)pScanValue);
        case SCAN_TYPE_U32:     return *(uint32_t *)pMemoryValue == (*(uint32_t *)pExtraValue + *(uint32_t *)pScanValue);
        case SCAN_TYPE_I32:      return *(int32_t *)pMemoryValue == (*(int32_t *)pExtraValue + *(int32_t *)pScanValue);
        case SCAN_TYPE_U64:     return *(uint64_t *)pMemoryValue == (*(uint64_t *)pExtraValue + *(uint64_t *)pScanValue);
        case SCAN_TYPE_I64:      return *(int64_t *)pMemoryValue == (*(int64_t *)pExtraValue + *(int64_t *)pScanValue);
        case SCAN_TYPE_FLOAT:      return *(float *)pMemoryValue == (*(float *)pExtraValue + *(float *)pScanValue);
        case SCAN_TYPE_DOUBLE:     return *(double *)pMemoryValue == (*(double *)pExtraValue + *(float *)pScanValue);
        case SCAN_TYPE_BYTE_ARRAY:
        case SCAN_TYPE_STRING:
        default:
            return FALSE;
    };
}

int compare_value_decreased(enum cmd_proc_scan_valuetype valType, BYTE *pScanValue, BYTE *pMemoryValue, BYTE *pExtraValue) {
    switch (valType) {
        case SCAN_TYPE_U8:      return *pMemoryValue < *pExtraValue;
        case SCAN_TYPE_I8:       return *(int8_t *)pMemoryValue < *(int8_t *)pExtraValue;
        case SCAN_TYPE_U16:     return *(uint16_t *)pMemoryValue < *(uint16_t *)pExtraValue;
        case SCAN_TYPE_I16:      return *(int16_t *)pMemoryValue < *(int16_t *)pExtraValue;
        case SCAN_TYPE_U32:     return *(uint32_t *)pMemoryValue < *(uint32_t *)pExtraValue;
        case SCAN_TYPE_I32:      return *(int32_t *)pMemoryValue < *(int32_t *)pExtraValue;
        case SCAN_TYPE_U64:     return *(uint64_t *)pMemoryValue < *(uint64_t *)pExtraValue;
        case SCAN_TYPE_I64:      return *(int64_t *)pMemoryValue < *(int64_t *)pExtraValue;
        case SCAN_TYPE_FLOAT:      return *(float *)pMemoryValue < *(float *)pExtraValue;
        case SCAN_TYPE_DOUBLE:     return *(double *)pMemoryValue < *(double *)pExtraValue;
        case SCAN_TYPE_BYTE_ARRAY:
        case SCAN_TYPE_STRING:
        default:
            return FALSE;
    };
}

int compare_value_decreased_by(enum cmd_proc_scan_valuetype valType, BYTE *pScanValue, BYTE *pMemoryValue, BYTE *pExtraValue) {
    switch (valType) {
        case SCAN_TYPE_U8:         return *pMemoryValue == (*pExtraValue - *pScanValue);
        case SCAN_TYPE_I8:         return *(int8_t *)pMemoryValue == (*(int8_t *)pExtraValue - *(int8_t *)pScanValue);
        case SCAN_TYPE_U16:        return *(uint16_t *)pMemoryValue == (*(uint16_t *)pExtraValue - *(uint16_t *)pScanValue);
        case SCAN_TYPE_I16:        return *(int16_t *)pMemoryValue == (*(int16_t *)pExtraValue - *(int16_t *)pScanValue);
        case SCAN_TYPE_U32:        return *(uint32_t *)pMemoryValue == (*(uint32_t *)pExtraValue - *(uint32_t *)pScanValue);
        case SCAN_TYPE_I32:        return *(int32_t *)pMemoryValue == (*(int32_t *)pExtraValue - *(int32_t *)pScanValue);
        case SCAN_TYPE_U64:        return *(uint64_t *)pMemoryValue == (*(uint64_t *)pExtraValue - *(uint64_t *)pScanValue);
        case SCAN_TYPE_I64:        return *(int64_t *)pMemoryValue == (*(int64_t *)pExtraValue - *(int64_t *)pScanValue);
        case SCAN_TYPE_FLOAT:      return *(float *)pMemoryValue == (*(float *)pExtraValue - *(float *)pScanValue);
        case SCAN_TYPE_DOUBLE:     return *(double *)pMemoryValue == (*(double *)pExtraValue - *(float *)pScanValue);
        case SCAN_TYPE_BYTE_ARRAY:
        case SCAN_TYPE_STRING:
        default:
            return FALSE;
    };
}

int compare_value_changed(enum cmd_proc_scan_valuetype valType, BYTE *pScanValue, BYTE *pMemoryValue, BYTE *pExtraValue) {
    switch (valType) {
        case SCAN_TYPE_U8:      return *pMemoryValue != *pExtraValue;
        case SCAN_TYPE_I8:       return *(int8_t *)pMemoryValue != *(int8_t *)pExtraValue;
        case SCAN_TYPE_U16:     return *(uint16_t *)pMemoryValue != *(uint16_t *)pExtraValue;
        case SCAN_TYPE_I16:      return *(int16_t *)pMemoryValue != *(int16_t *)pExtraValue;
        case SCAN_TYPE_U32:     return *(uint32_t *)pMemoryValue != *(uint32_t *)pExtraValue;
        case SCAN_TYPE_I32:      return *(int32_t *)pMemoryValue != *(int32_t *)pExtraValue;
        case SCAN_TYPE_U64:     return *(uint64_t *)pMemoryValue != *(uint64_t *)pExtraValue;
        case SCAN_TYPE_I64:      return *(int64_t *)pMemoryValue != *(int64_t *)pExtraValue;
        case SCAN_TYPE_FLOAT:      return *(float *)pMemoryValue != *(float *)pExtraValue;
        case SCAN_TYPE_DOUBLE:     return *(double *)pMemoryValue != *(double *)pExtraValue;
        case SCAN_TYPE_BYTE_ARRAY:
        case SCAN_TYPE_STRING:
        default:
            return FALSE;
    };
}

int compare_value_unchanged(enum cmd_proc_scan_valuetype valType, BYTE *pScanValue, BYTE *pMemoryValue, BYTE *pExtraValue) {
    switch (valType) {
        case SCAN_TYPE_U8:     return *pMemoryValue == *pExtraValue;
        case SCAN_TYPE_I8:      return *(int8_t *)pMemoryValue == *(int8_t *)pExtraValue;
        case SCAN_TYPE_U16:    return *(uint16_t *)pMemoryValue == *(uint16_t *)pExtraValue;
        case SCAN_TYPE_I16:     return *(int16_t *)pMemoryValue == *(int16_t *)pExtraValue;
        case SCAN_TYPE_U32:    return *(uint32_t *)pMemoryValue == *(uint32_t *)pExtraValue;
        case SCAN_TYPE_I32:     return *(int32_t *)pMemoryValue == *(int32_t *)pExtraValue;
        case SCAN_TYPE_U64:    return *(uint64_t *)pMemoryValue == *(uint64_t *)pExtraValue;
        case SCAN_TYPE_I64:     return *(int64_t *)pMemoryValue == *(int64_t *)pExtraValue;
        case SCAN_TYPE_FLOAT:     return *(float *)pMemoryValue == *(float *)pExtraValue;
        case SCAN_TYPE_DOUBLE:    return *(double *)pMemoryValue == *(double *)pExtraValue;
        case SCAN_TYPE_BYTE_ARRAY:
        case SCAN_TYPE_STRING:
        default:
            return FALSE;
    };
}
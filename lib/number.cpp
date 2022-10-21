#include "number.h"
#include <cstring>
#include <cmath>

uint32_t NumbersInt32(int i) {
    int value = 0;
    while (i) {
        i /= 10;
        value++;
    }

    return value;
}

uint32_t NumbersInt2022(const uint2022_t i) {
    for (int j = 0; j < kPartsInUint2022; j++) {
        if (i.parts[j]) {
            return NumbersInt32(i.parts[j]) + kDigitsInPart * (kPartsInUint2022 - 1 - j);
        }
    }

    return 0;
}

void TransferOne(uint32_t index, uint2022_t& value){
    if (value.parts[index] >= kBillion) {
        value.parts[index] -= kBillion;
        value.parts[index - 1] += 1;
    }
}

uint2022_t from_uint(uint32_t i) {
    uint2022_t value;
    std::memset(value.parts, 0, sizeof(uint2022_t));
    value.parts[kPartsInUint2022 - 1] = i;

    return value;
}

uint2022_t from_string(const char* buff) {
    uint2022_t value;
    std::memset(value.parts, 0, sizeof(uint2022_t));
    uint32_t position = 1;
    uint32_t part = kPartsInUint2022 - 1;
    value.parts[part] = 0;
    for (int i = std::strlen(buff) - 1; i >= 0; i--) {
        if (position == kBillion) {
            position = 1;
            part--;
        }
        value.parts[part] += (buff[i] - '0') * position;
        position *= 10;
    }

    return value;
}

uint2022_t operator+(const uint2022_t& lhs, const uint2022_t& rhs) {
    uint2022_t result;
    std::memset(result.parts, 0, sizeof(uint2022_t));
    for (int part = kPartsInUint2022 - 1; part >= 0; part--) {
        result.parts[part] += lhs.parts[part] + rhs.parts[part];
        TransferOne(part, result);
    }

    return result;
}

uint2022_t operator-(const uint2022_t& lhs, const uint2022_t& rhs) {
    uint2022_t result;
    std::memset(result.parts, 0, sizeof(uint2022_t));
    for (int part = kPartsInUint2022 - 1; part >= 0; part--) {
        result.parts[part] += lhs.parts[part] - rhs.parts[part];
        if (result.parts[part] < 0) {
            result.parts[part - 1] -= 1;
            result.parts[part] += kBillion;
        }
    }

    return result;
}

uint2022_t operator*(const uint2022_t& lhs, const uint2022_t& rhs) {
    uint2022_t result;
    std::memset(result.parts, 0, sizeof(uint2022_t));
    for (int i = 0; i <= kPartsInUint2022 - 1; i++) {
        if (lhs.parts[kPartsInUint2022 - 1 - i]) {
            for (int j = 0; j <= kPartsInUint2022 - 1; j++) {
                if (rhs.parts[kPartsInUint2022 - 1 - j]) {
                    long long res = static_cast<long long>(lhs.parts[kPartsInUint2022 - 1 - i]) *
                                    rhs.parts[kPartsInUint2022 - 1 - j];
                    result.parts[kPartsInUint2022 - 1 - i - j - 1] += res / kBillion;
                    result.parts[kPartsInUint2022 - 1 - i - j] += res % kBillion;
                    TransferOne(kPartsInUint2022 - 1 - i - j, result);
                    TransferOne(kPartsInUint2022 - 1 - i - j - 1, result);
                }
            }
        }
    }

    return result;
}

uint2022_t operator/(uint2022_t lhs, uint2022_t& rhs) {
    uint2022_t result;
    std::memset(result.parts, 0, sizeof(uint2022_t));
    int result_numbers = NumbersInt2022(lhs) - NumbersInt2022(rhs);
    if (result_numbers < 0) {
        return result;
    }
    if (!NumbersInt2022(rhs)) {  // проверяем деление на ноль
        return rhs;
    }
    for (int i = 0; i <= kPartsInUint2022 - 1 - result_numbers / kDigitsInPart; i++) {
        // в этом цикле мы подгоняем rhs к lhs по количаству разрядов
        uint64_t tmp = (uint64_t) rhs.parts[i + result_numbers / kDigitsInPart] * (uint32_t) std::pow(10, result_numbers % kDigitsInPart);
        rhs.parts[i] = tmp % kBillion;
        if (i) {
            rhs.parts[i - 1] += tmp / kBillion;
        }
        if (kPartsInUint2022 - 1 - result_numbers / kDigitsInPart * 2 < i) {
            rhs.parts[i + result_numbers / kDigitsInPart] = 0;
        }
    }
    while (result_numbers >= 0) {
        while (lhs >= rhs) {
            result.parts[kPartsInUint2022 - 1 - result_numbers / kDigitsInPart]++;
            lhs = lhs - rhs;
        }
        if (result_numbers % kDigitsInPart) {  // освобождаем место для записи нового разряда результата
            result.parts[kPartsInUint2022 - 1 - result_numbers / kDigitsInPart] *= 10;
        }
        for (int i = kPartsInUint2022 - 1; i >= 0; i--) {  // делим rhs на 10
            rhs.parts[i] /= 10;
            if (i) {
                rhs.parts[i] += (rhs.parts[i - 1] % 10) * kBillion;
            }
        }
        result_numbers--;
    }

    return result;
}

std::ostream& operator<<(std::ostream& stream, const uint2022_t& value) {
    bool only_zeros = true;
    for (auto i: value.parts) {
        if (!only_zeros) {
            for (int j = 0; j + NumbersInt32(i) < kDigitsInPart; j++) {
                stream << 0;
            }
        }
        if (i) {
            stream << i;
            only_zeros = false;
        }
    }
    if (only_zeros) stream << 0;

    return stream;
}

bool operator==(const uint2022_t& lhs, const uint2022_t& rhs) {
    for (int part = kPartsInUint2022 - 1; part >= 0; part--) {
        if (lhs.parts[part] != rhs.parts[part]) {
            return false;
        }
    }

    return true;
}

bool operator!=(const uint2022_t& lhs, const uint2022_t& rhs) {
    return !(lhs == rhs);
}

bool operator>(const uint2022_t& lhs, const uint2022_t& rhs) {
    for (int part = 0; part <= kPartsInUint2022 - 1; part++) {
        if (lhs.parts[part] > rhs.parts[part]) {
            return true;
        } else if (lhs.parts[part] < rhs.parts[part]) {
            return false;
        }
    }

    return false;
}

bool operator>=(const uint2022_t& lhs, const uint2022_t& rhs) {
    if (lhs > rhs or lhs == rhs) {
        return true;
    }

    return false;
}

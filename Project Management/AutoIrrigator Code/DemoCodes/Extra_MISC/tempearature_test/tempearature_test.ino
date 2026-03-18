#include <vector>

struct TempValue {
    int value;
    size_t index;
};

std::vector<int> temp_table = {
    994, 992, 990, 988, 986, 983, 981, 978, 975, 972, 969, 966, 962, 959, 955, 951, 947, 943, 938, 933,
    928, 923, 918, 912, 907, 901, 894, 888, 881, 874, 867, 860, 852, 845, 837, 828, 820, 811, 802, 793,
    784, 774, 765, 755, 745, 735, 724, 714, 703, 692, 682, 671, 659, 648, 637, 626, 614, 603, 592, 580,
    569, 557, 546, 535, 523, 512, 501, 490, 479, 468, 457, 446, 436, 425, 415, 405, 395, 385, 375, 365,
    356, 347, 338, 329, 320, 311, 303, 295, 287, 279, 271, 264, 256, 249, 242, 235, 229, 222, 216, 210,
    204, 198, 193, 187, 182, 176, 172, 167, 162, 157, 153, 148, 144, 140, 136, 132, 128, 125, 121, 118,
    114, 111, 108, 105, 102, 99, 96, 94, 91, 88, 86, 84, 81, 79, 77, 75, 73, 71, 69, 67, 65, 63, 62, 60,
    58, 57, 55, 54, 52, 51, 50, 48, 47, 46, 45, 44, 43, 41, 40, 39, 38, 37, 36, 36, 35, 34, 33, 32, 31, 31,
    30, 29, 28, 28, 27, 26, 26, 25, 25, 24, 23, 23, 22, 22, 21, 21, 20, 20, 20, 19, 19
};

int custom_abs(int x) {
    return (x >= 0) ? x : -x;
}


TempValue closestNumber(const std::vector<int>& temp_table, int value) {
    TempValue result;
    result.value = temp_table[0];
    result.index = 0;

    int min_diff = custom_abs(temp_table[0] - value);

    for (size_t i = 1; i < temp_table.size(); ++i) {
        int diff = custom_abs(temp_table[i] - value);
        if (diff < min_diff) {
            min_diff = diff;
            result.value = temp_table[i];
            result.index = i;
        }
    }

    result.index -= 40;

    return result;
}

void setup() {

Serial.begin(9600);


}

void loop() {

    TempValue closest = closestNumber(temp_table, 100);
Serial.println(100);
Serial.println(closest.index);

    closest = closestNumber(temp_table, 200);
    Serial.println(200);
Serial.println(closest.index);
     delay(500);  // delay in between reads for stability

}

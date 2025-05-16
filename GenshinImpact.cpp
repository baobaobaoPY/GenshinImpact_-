#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cstring>


// 物品池定义
const std::vector<std::string> UP_5Stars = {"爱可菲"};
const std::vector<std::string> UP_5Stars1 = {"娜维娅"};
const std::vector<std::string> NOUP_5Stars = {"爱可菲", "琴", "迪卢克", "莫娜", "刻晴", "七七", "提纳里", "迪希雅", "梦见月瑞希"};
const std::vector<std::string> NOUP_5Stars1 = {"娜维娅", "琴", "迪卢克", "莫娜", "刻晴", "七七", "提纳里", "迪希雅", "梦见月瑞希"};
const std::vector<std::string> UP_4Stars = {"伊法", "欧洛伦", "莱依拉"};
const std::vector<std::string> NOUP_4Stars_NAME = {"塔利雅", "伊安珊", "蓝砚", "卡齐娜", "赛索斯", "嘉明", "夏沃蕾", "夏洛蒂", "菲米尼", "琳妮特", 
                                        "卡维", "米卡", "瑶瑶", "珐露珊", "坎蒂丝", "多莉", "柯莱", "久岐忍", "云堇", "绮良良", "鹿野院平藏", 
                                        "九条裟罗", "五郎", "早柚", "托马", "烟绯", "罗莎莉亚", "辛焱", "砂糖", "迪奥娜", "重云", "诺艾尔", 
                                        "班尼特", "菲谢尔", "凝光", "行秋", "北斗", "香菱", "雷泽", "芭芭拉"};

const std::vector<std::string> NOUP_4Stars_WEAPON = {"匣里龙吟", "祭礼剑", "笛剑", "西风剑", "雨裁", "祭礼大剑", "钟剑", "西风大剑",
                                        "西风长枪", "匣里灭辰", "昭心", "祭礼残章", "流浪乐章", "西风秘典", "弓藏",
                                        "祭礼弓", "绝弦", "西风猎弓"};

const std::vector<std::string> BLUE_ITEMS = {"飞天御剑", "黎明神剑", "冷刃", "以理服人", "沐浴龙血的剑", "铁影阔剑", "黑缨枪",
                                        "翡玉法球", "讨龙英杰谭", "魔导绪论", "弹弓", "神射手之誓", "鸦羽弓"};

class RoleGachaSystem {
private:
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;
    
    int post_5star_counter;  // 记录五星出现后的抽卡次数
    int cishu;
    int four_star_counter;
    int wai_5star;
    int wai_4star;
    std::string last_nonup_4star;
    bool targeted_UP;
    
    double _calculate_5star_probability() {
        if (cishu <= 73) return 0.6438;
        else if (cishu <= 89) return 0.6 + (cishu - 73) * 6;
        else return 100.0;
    }

    double _calculate_4star_probability() {
        if (four_star_counter < 10) {
            // 如果在五星后的10抽内则概率翻倍
            return (post_5star_counter >= 0 && post_5star_counter < 10) ? 11.124 : 5.1;
            }
        return 100.0;
        }

    int _handle_5star() {
        if (wai_5star == 1) {
            wai_5star = 0;
            return 0;
        }
        int result = (dis(gen) < 0.5) ? 1 : 0;
        if (result == 1) wai_5star = 1;
        return result;
    }

    int _handle_4star() {
        if (wai_4star == 1 || dis(gen) < 0.5) {
            wai_4star = 0;
            return 2;
        }
        wai_4star = 1;
        std::vector<std::string> valid_chars;
        if (dis(gen) < 0.5) {
            std::copy_if(NOUP_4Stars_NAME.begin(), NOUP_4Stars_NAME.end(), std::back_inserter(valid_chars),
                   [](const std::string& s) { return std::find(UP_4Stars.begin(), UP_4Stars.end(), s) == UP_4Stars.end(); });
            if (!valid_chars.empty()) {
                std::uniform_int_distribution<> d(0, valid_chars.size()-1);
                last_nonup_4star = valid_chars[d(gen)];
            }
        } else {
            std::uniform_int_distribution<> d(0, NOUP_4Stars_WEAPON.size()-1);
            last_nonup_4star = NOUP_4Stars_WEAPON[d(gen)];
        }
        return 3;
    }

    std::string _get_item(int result_type) {
        if (result_type == 0) {
            auto& pool = targeted_UP ? UP_5Stars1 : UP_5Stars;
            std::uniform_int_distribution<> d(0, pool.size()-1);
            return pool[d(gen)];
        }
        if (result_type == 1) {
            auto& pool = targeted_UP ? NOUP_5Stars1 : NOUP_5Stars;
            std::uniform_int_distribution<> d(0, pool.size()-1);
            return pool[d(gen)];
        }
        if (result_type == 2) {
            std::uniform_int_distribution<> d(0, UP_4Stars.size()-1);
            return UP_4Stars[d(gen)];
        }
        if (result_type == 3) return last_nonup_4star;
        
        std::uniform_int_distribution<> d(0, BLUE_ITEMS.size()-1);
        return BLUE_ITEMS[d(gen)];
    }

public:
    RoleGachaSystem() : gen(std::random_device{}()), dis(0.0, 1.0) {
        reset();
        targeted_UP = false;
        post_5star_counter = -1;
    }

    void reset() {
        cishu = 0;
        four_star_counter = 0;
        wai_5star = 0;
        wai_4star = 0;
        last_nonup_4star = "";
        post_5star_counter = -1;
    }

    std::pair<std::string, int> pull() {
        if (post_5star_counter >= 0) {
            if (++post_5star_counter >= 10) {
                post_5star_counter = -1;
            }
        }

        cishu++;
        four_star_counter++;

        // 强制保底判定
        if (cishu >= 89) {
            auto item = _get_item(_handle_5star());
            cishu = 0;
            post_5star_counter = 0;
            return {item, 0};
        }
        if (four_star_counter >= 10) {
            auto item = _get_item(_handle_4star());
            four_star_counter = 0;
            return {item, 2};
        }

        double p5 = _calculate_5star_probability();
        double p4 = _calculate_4star_probability();
        double rand_val = dis(gen) * 100;

        if (rand_val < p5) {
            auto item = _get_item(_handle_5star());
            cishu = 0;
            post_5star_counter = 0;
            return {item, 0};
        }
        if (rand_val < (p5 + p4)) {
            auto item = _get_item(_handle_4star());
            four_star_counter = 0;
            return {item, 2};
        }
        
        std::uniform_int_distribution<> d(0, BLUE_ITEMS.size()-1);
        return {BLUE_ITEMS[d(gen)], 4};
    }

    std::vector<std::pair<std::string, int>> multi_pull(int count) {
        std::vector<std::pair<std::string, int>> results;
        while (count-- > 0) {
            results.push_back(pull());
        }
        return results;
    }

    void set_targeted_UP(bool targeted) {
        this->targeted_UP = targeted;
    }
};

extern "C" {
    // 导出函数，用于创建抽卡系统
    __declspec(dllexport) void* CreateGachaSystem() {
        return new RoleGachaSystem();
    }

    // 导出函数，用于销毁抽卡系统
    __declspec(dllexport) void DestroyGachaSystem(void* system) {
        delete static_cast<RoleGachaSystem*>(system);
    }

    // 导出函数，用于设置定轨
    __declspec(dllexport) void SetTargetedWeapon(void* system, int targeted) {
        static_cast<RoleGachaSystem*>(system)->set_targeted_UP(targeted);
    }

    // 导出函数，用于单次抽卡
    __declspec(dllexport) void DoPull(void* system, char* item_name, int* rarity) {
        auto result = static_cast<RoleGachaSystem*>(system)->pull();
        strncpy_s(item_name, 256, result.first.c_str(), _TRUNCATE); // 使用安全的strncpy_s
        *rarity = result.second;
    }

    // 导出函数，用于批量抽卡
    __declspec(dllexport) int MultiPull(void* system, int count, char* buffer, int buffer_size) {
        auto results = static_cast<RoleGachaSystem*>(system)->multi_pull(count);
        std::string result_str;
        for (const auto& result : results) {
            result_str += result.first + "," + std::to_string(result.second) + "|";
        }
        result_str = result_str.substr(0, result_str.size() - 1); // 去掉最后一个 | 
        strncpy(buffer, result_str.c_str(), buffer_size);
        buffer[buffer_size - 1] = '\0';
        return results.size();
    }
}

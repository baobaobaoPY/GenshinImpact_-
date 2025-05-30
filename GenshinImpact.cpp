// Genshin_Impact 角色抽卡系统模块，通过C++动态链接库(DLL)提供高质量的生成算法，
// 支持多语言集成（Python/C#/Java/...），精确模拟原神中的抽卡概率机制，
// 本文件遵循 GPLv3 开源协议 - 修改代码版本必须开源并保留此声明，
// 您可以分发包含此声明的衍生作品，但禁止修改此标题内容！
// Genshin_Impact Character Gacha System Module - Provides high-quality generation algorithms via C++ DLL (Dynamic Link Library)
// Supports multi-language integration (Python/C#/Java/...) with precise simulation of the gacha probability mechanisms from Genshin Impact
// This file is licensed under GPLv3 open-source license - Modified versions MUST be open-sourced and retain this declaration
// You may distribute derivative works containing this notice, but modification of THIS TITLE HEADER is STRICTLY PROHIBITED!


#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cstring>


// 物品池定义
const std::vector<std::string> UP_5Stars = {"丝柯克"};  // 当期UP1五星角色池
const std::vector<std::string> UP_5Stars1 = {"艾梅莉埃"};  // 当期UP2五星角色池
const std::vector<std::string> NOUP_5Stars = {"琴", "迪卢克", "莫娜", "刻晴", "七七", "提纳里", "迪希雅", "梦见月瑞希"};

const std::vector<std::string> UP_4Stars = {"塔利雅", "行秋", "芭芭拉"};  // 当期UP四星角色列表
const std::vector<std::string> NOUP_4Stars_NAME = {"伊法", "伊安珊", "蓝砚", "欧洛伦", "卡齐娜", "赛索斯", "嘉明", 
                                        "夏沃蕾", "夏洛蒂", "菲米尼", "琳妮特", "卡维", "米卡", "瑶瑶", "莱依拉", "珐露珊", 
                                        "坎蒂丝", "多莉", "柯莱", "久岐忍", "云堇", "绮良良", "鹿野院平藏", "五郎", "九条裟罗", 
                                        "早柚", "托马", "烟绯", "罗莎莉亚", "辛焱", "砂糖", "迪奥娜", "重云", "诺艾尔", "班尼特", 
                                        "凝光", "北斗", "香菱", "雷泽", "菲谢尔"};
const std::vector<std::string> NOUP_4Stars_WEAPON = {"匣里龙吟", "祭礼剑", "笛剑", "西风剑", "雨裁", "祭礼大剑", "钟剑", "西风大剑",
                                        "西风长枪", "匣里灭辰", "昭心", "祭礼残章", "流浪乐章", "西风秘典", "弓藏",
                                        "祭礼弓", "绝弦", "西风猎弓"};

const std::vector<std::string> BLUE_ITEMS = {"飞天御剑", "黎明神剑", "冷刃", "以理服人", "沐浴龙血的剑", "铁影阔剑", "黑缨枪",
                                        "翡玉法球", "讨龙英杰谭", "魔导绪论", "弹弓", "神射手之誓", "鸦羽弓"};


class RoleGachaSystem {
private:  // 定义所需的变量与函数
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;
    int post_5star_counter;
    int cishu;  // 抽取次数
    int four_star_counter;
    int wai_5star;  // 定义记录五星物品出现非UP物品的变量
    int wai_4star;  // 定义记录四星物品出现非UP物品的变量
    std::string last_nonup_4star;  // 存储非UP四星物品的变量
    bool targeted_UP;  // 确定是否进行UP物品抽取
    int capture;  // 定义记录是否进行非UP物品抽取的变量
    int _Temp_;  // 定义决定转非为定的捕获明光判断逻辑临时变量

    double _calculate_5star_probability() {
        if (cishu <= 73) return 0.6438;  // 定义五星基础生成概率
        else if (cishu <= 89) return 0.6 + (cishu - 73) * 6;  // 定义概率随抽卡次数增加而缓慢提升
        else return 100.0;  // 90抽时返回100%概率
    }

    double _calculate_4star_probability() {
        if (four_star_counter < 10) {  // 定义四星基础生成概率
            // 定义在五星物品生成后，四星物品概率提升至原本概率的双倍 + 0.924
            return (post_5star_counter >= 0 && post_5star_counter < 10) ? 11.124 : 5.1;}
        return 100.0;  // 10抽或大于10抽时返回100%概率
    }

    int _handle_5star() {
        // 通过返回值判断是否歪掉当期UP角色
        if (wai_5star == 1) {
            wai_5star = 0;
            return 0;
        }
        int result = (dis(gen) < 0.5) ? 1 : 0;  // 50%概率返回1，50%概率返回0
        if (result == 1) wai_5star = 1;
        return result;
    }

    int _handle_4star() {
        // 通过返回值判断是否歪掉当期UP四星物品并进入4星物品歪掉抽取的流程中
        if (wai_4star == 1 || dis(gen) < 0.5) {  // 50%概率返回2，50%概率返回3
            wai_4star = 0;
            return 2;
        }
        wai_4star = 1;
        std::vector<std::string> valid_chars;
        if (dis(gen) < 0.5) {  // 50%概率返回非UP四星角色或非UP四星武器
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
            std::uniform_int_distribution<> d(0, NOUP_5Stars.size()-1);
            return NOUP_5Stars[d(gen)];
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
        capture = 0;
        _Temp_ = 0;
    }

    void reset() {
        cishu = 0;
        four_star_counter = 0;
        wai_5star = 0;
        wai_4star = 0;
        last_nonup_4star = "";
        post_5star_counter = -1;
        capture = 0;
        _Temp_ = 0;
    }

    std::pair<std::string, int> pull() {
        if (post_5star_counter >= 0) {
            if (++post_5star_counter >= 10) {
                post_5star_counter = -1;
            }
        }
        cishu++;  // 进入生成物品前自增抽卡次数（有效防止逻辑错误与混乱）
        four_star_counter++;  // 进入生成物品前自增四星物品计数器

        // 强制保底判定
        if (cishu >= 89) {  // 受四星保底影响后的89抽时强制五星提前保底
            int result_type = _handle_5star();
            auto item = _get_item(result_type);
            bool is_noup = (result_type == 1);

            if (is_noup) {  // 捕获明光实现
                capture++;  // 捕获明光变量自增
                _Temp_ = 0;  // 临时变量，决定是否捕获非UP五星物品为当期UP五星物品

                if (capture == 3) {  // 捕获明光增加至三次时，将非UP五星物品作为当期UP五星物品返回
                    std::uniform_int_distribution<> d(0, (targeted_UP ? UP_5Stars1.size() : UP_5Stars.size()) - 1);
                    item = (targeted_UP ? UP_5Stars1[d(gen)] : UP_5Stars[d(gen)]);
                    capture = 0;
                } else if (capture == 2) {  // 捕获明光增加至两次时，将非UP五星物品有10%的概率作为当期UP五星物品返回
                    std::bernoulli_distribution dist(0.10);  // 定义概率为10%
                    _Temp_ = dist(gen) ? 1 : 0;
                    if (_Temp_ == 1) {
                        std::uniform_int_distribution<> d(0, (targeted_UP ? UP_5Stars1.size() : UP_5Stars.size()) - 1);
                        item = (targeted_UP ? UP_5Stars1[d(gen)] : UP_5Stars[d(gen)]);
                        capture = 0;
                    }
                } else if (capture == 1) {  // 捕获明光增加至一次时，将非UP五星物品有0.018%的概率作为当期UP五星物品返回
                    std::bernoulli_distribution dist(0.00018);  // 定义概率为0.018%
                    _Temp_ = dist(gen) ? 1 : 0;
                    if (_Temp_ == 1) {
                        std::uniform_int_distribution<> d(0, (targeted_UP ? UP_5Stars1.size() : UP_5Stars.size()) - 1);
                        item = (targeted_UP ? UP_5Stars1[d(gen)] : UP_5Stars[d(gen)]);
                        capture = 0;  // 出现捕获明光后将捕获明光变量次数归零
                    }
                }
            }

            cishu = 0;  // 生成五星物品后将抽取次数归零
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
            int result_type = _handle_5star();
            auto item = _get_item(result_type);
            bool is_noup = (result_type == 1);

            if (is_noup) {  // 捕获明光实现
                capture++;  // 捕获明光变量自增
                _Temp_ = 0;  // 临时变量，决定是否捕获非UP五星物品为当期UP五星物品

                if (capture == 3) {  // 捕获明光增加至三次时，将非UP五星物品作为当期UP五星物品返回
                    std::uniform_int_distribution<> d(0, (targeted_UP ? UP_5Stars1.size() : UP_5Stars.size()) - 1);
                    item = (targeted_UP ? UP_5Stars1[d(gen)] : UP_5Stars[d(gen)]);
                    capture = 0;
                } else if (capture == 2) {  // 捕获明光增加至两次时，将非UP五星物品有10%的概率作为当期UP五星物品返回
                    std::bernoulli_distribution dist(0.10);  // 定义概率为10%
                    _Temp_ = dist(gen) ? 1 : 0;
                    if (_Temp_ == 1) {
                        std::uniform_int_distribution<> d(0, (targeted_UP ? UP_5Stars1.size() : UP_5Stars.size()) - 1);
                        item = (targeted_UP ? UP_5Stars1[d(gen)] : UP_5Stars[d(gen)]);
                        capture = 0;
                    }
                } else if (capture == 1) {  // 捕获明光增加至一次时，将非UP五星物品有0.018%的概率作为当期UP五星物品返回
                    std::bernoulli_distribution dist(0.00018);  // 定义概率为0.018%
                    _Temp_ = dist(gen) ? 1 : 0;
                    if (_Temp_ == 1) {
                        std::uniform_int_distribution<> d(0, (targeted_UP ? UP_5Stars1.size() : UP_5Stars.size()) - 1);
                        item = (targeted_UP ? UP_5Stars1[d(gen)] : UP_5Stars[d(gen)]);
                        capture = 0;  // 出现捕获明光后将捕获明光变量次数归零
                    }
                }
            }

            cishu = 0;  // 生成五星物品后将抽取次数归零
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

extern "C" {  // 导出函数（提供C++编译dll动态链接接口）
    __declspec(dllexport) void* CreateGachaSystem() {
        return new RoleGachaSystem();
    }

    __declspec(dllexport) void DestroyGachaSystem(void* system) {
        delete static_cast<RoleGachaSystem*>(system);
    }

    __declspec(dllexport) void SetTargetedWeapon(void* system, int targeted) {
        static_cast<RoleGachaSystem*>(system)->set_targeted_UP(targeted);
    }

    __declspec(dllexport) void DoPull(void* system, char* item_name, int* rarity) {
        auto result = static_cast<RoleGachaSystem*>(system)->pull();
        strncpy_s(item_name, 256, result.first.c_str(), _TRUNCATE);
        *rarity = result.second;
    }

    __declspec(dllexport) int MultiPull(void* system, int count, char* buffer, int buffer_size) {
        auto results = static_cast<RoleGachaSystem*>(system)->multi_pull(count);
        std::string result_str;
        for (const auto& result : results) {
            result_str += result.first + "," + std::to_string(result.second) + "|";
        }
        result_str = result_str.substr(0, result_str.size() - 1);
        strncpy(buffer, result_str.c_str(), buffer_size);
        buffer[buffer_size - 1] = '\0';
        return results.size();
    }
}

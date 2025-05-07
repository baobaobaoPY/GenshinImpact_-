# 需要 1920×1080 分辨率及以上！
import sys
import random
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QPushButton, QLabel, QComboBox, QMessageBox, QTextEdit


class WeaponGachaSystem:
    def __init__(self):
        # 物品池配置
        self.UP_5STAR = ["岩峰巡歌"]
        self.UP_5STAR1 = ["终末嗟叹之诗"]
        self.NOUP_5STAR = ["终末嗟叹之诗", "天空之刃", "风鹰剑", "狼的末路", "天空之傲", "和璞鸢",
                           "天空之脊", "四风原典", "天空之卷", "阿莫斯之弓", "天空之翼"]
        self.NOUP_5STAR1 = ["岩峰巡歌", "天空之刃", "风鹰剑", "狼的末路", "天空之傲", "和璞鸢",
                            "天空之脊", "四风原典", "天空之卷", "阿莫斯之弓", "天空之翼"]
        self.UP_4STAR = ["祭礼剑", "钟剑", "西风长枪", "西风秘典", "西风猎弓"]
        self.NOUP_4STAR_CHAR = ["塔利雅", "伊法", "伊安珊", "嘉明", "夏沃蕾", "蓝砚",
                                "欧洛伦", "卡齐娜", "赛索斯", "夏洛蒂", "菲米尼", "琳妮特",
                                "卡维", "米卡", "瑶瑶", "珐露珊", "莱依拉", "坎蒂丝",
                                "多莉", "柯莱", "久岐忍", "云堇", "绮良良", "鹿野院平藏",
                                "九条裟罗", "五郎", "早柚", "托马", "烟绯", "罗莎莉亚",
                                "辛焱", "砂糖", "迪奥娜", "重云", "诺艾尔", "班尼特",
                                "菲谢尔", "凝光", "行秋", "北斗", "香菱", "雷泽", "芭芭拉"]
        self.NOUP_4STAR_WEAPON = ['匣里龙吟', '笛剑', '西风剑', '雨裁', '祭礼大剑',
                                  '西风大剑', '匣里灭辰', '昭心', '祭礼残章', '流浪乐章',
                                  '弓藏', '祭礼弓', '绝弦']
        self.BLUE_ITEMS = ["飞天御剑", "黎明神剑", "冷刃", "以理服人", "沐浴龙血的剑",
                           "铁影阔剑", "黑缨枪", "翡玉法球", "讨龙英杰谭", "魔导绪论",
                           "弹弓", "神射手之誓", "鸦羽弓"]

        self.reset()
        self.targeted_UP = 0  # 初始值改为0（0=不定轨，1=定轨UP_5STAR，2=定轨UP_5STAR1）

    def reset(self):
        """重置所有抽卡状态"""
        self.gold_pity = 0  # 五星保底计数
        self.purple_pity = 0  # 四星保底计数
        self.gold_guarantee = 0  # 五星保底状态 0=可能歪 1=必出UP
        self.purple_guarantee = 0  # 四星保底状态
        self.last_purple_type = ""  # 记录上次四星类型(character/weapon)

    def set_targeted_weapon(self, target_code: int):
        """设置定轨武器（接口参数改为int类型）"""
        if target_code in (0, 1, 2):
            self.targeted_UP = target_code
        else:
            sys.exit()

    def _calc_gold_prob(self):
        """计算当前五星概率"""
        if self.gold_pity <= 62:
            return 0.652
        elif self.gold_pity <= 73:
            return 0.7 + (self.gold_pity - 62) * 7
        elif self.gold_pity <= 79:
            base_prob_73 = 0.7 + (73 - 62) * 7
            return base_prob_73 + (self.gold_pity - 73) * 3.5
        else:
            return 100.0

    def _calc_purple_prob(self):
        """计算当前四星概率"""
        if self.purple_pity <= 7:
            return 5.455
        elif self.purple_pity < 10:
            return 33.3 + (self.purple_pity - 7) * 33
        else:
            return 100.0

    def _get_gold_item(self):
        """获取五星物品（根据数字定轨选择池子）"""
        if self.targeted_UP not in (0, 1, 2):
            sys.exit()

        if self.gold_guarantee == 1 or random.random() < 0.75:
            self.gold_guarantee = 0
            if self.targeted_UP == 0:
                return random.choice(random.choice([self.UP_5STAR, self.UP_5STAR1])), 0
            elif self.targeted_UP == 1:
                return random.choice(self.UP_5STAR), 0
            else:
                return random.choice(self.UP_5STAR1), 0
        else:
            self.gold_guarantee = 1
            if self.targeted_UP == 0:
                return random.choice(random.choice([self.NOUP_5STAR, self.NOUP_5STAR1])), 1
            elif self.targeted_UP == 1:
                return random.choice(self.NOUP_5STAR), 1
            else:
                return random.choice(self.NOUP_5STAR1), 1

    def _get_purple_item(self):
        """获取四星物品"""
        if self.purple_guarantee == 1:
            item = random.choice(self.UP_4STAR)
            self.purple_guarantee = 0
            return item, 2

        if random.random() < 0.25:
            if random.random() < 0.5:
                pool = self.NOUP_4STAR_WEAPON
            else:
                pool = self.NOUP_4STAR_CHAR
            item = random.choice(pool)
            self.purple_guarantee = 1
            return item, 3
        else:
            item = random.choice(self.UP_4STAR)
            self.purple_guarantee = 0
            return item, 2

    def pull(self):
        """执行单次抽卡"""
        self.gold_pity += 1
        self.purple_pity += 1

        gold_prob = self._calc_gold_prob()
        purple_prob = self._calc_purple_prob()

        if self.gold_pity >= 79 or random.random() * 100 < gold_prob:
            item, result_type = self._get_gold_item()
            self.gold_pity = 0
            return item, result_type
        if self.purple_pity >= 10 or random.random() * 100 < purple_prob:
            item, result_type = self._get_purple_item()
            self.purple_pity = 0
            return item, result_type
        return random.choice(self.BLUE_ITEMS), 4

    def multi_pull(self, count=10):
        """执行多次抽卡"""
        return [self.pull() for _ in range(count)]

    @property
    def gold_pity(self):
        return self._gold_pity

    @gold_pity.setter
    def gold_pity(self, value):
        self._gold_pity = min(value, 80)

    @property
    def purple_pity(self):
        return self._purple_pity

    @purple_pity.setter
    def purple_pity(self, value):
        self._purple_pity = min(value, 10)


class GenshinGachaWindow(QMainWindow):
    def __init__(self, di):
        super().__init__()
        self.di = di
        self.gacha = WeaponGachaSystem()
        self.gacha.set_targeted_weapon(0)
        self.init_ui()
        self.total_pulls = 0
        self.current_count = 1

    def init_ui(self):
        self.setWindowTitle(f"原神模拟多人抽卡器 - UID: {self.di}")
        self.setGeometry(100, 100, 300, 888)

        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        layout = QVBoxLayout()

        self.di_label = QLabel(f"设备标识 (UID): {self.di}", self)
        layout.addWidget(self.di_label)

        self.result_text = QTextEdit(self)
        self.result_text.setReadOnly(True)
        layout.addWidget(self.result_text)

        self.track_combo = QComboBox(self)
        self.track_combo.addItem("不定轨")
        self.track_combo.addItem("岩峰巡歌")
        self.track_combo.addItem("终末嗟叹之诗")
        self.track_combo.currentIndexChanged.connect(self.change_track)
        layout.addWidget(self.track_combo)

        self.single_pull_btn = QPushButton("单抽", self)
        self.single_pull_btn.clicked.connect(self.single_pull)
        layout.addWidget(self.single_pull_btn)

        self.ten_pull_btn = QPushButton("十抽", self)
        self.ten_pull_btn.clicked.connect(self.ten_pull)
        layout.addWidget(self.ten_pull_btn)

        central_widget.setLayout(layout)

    def change_track(self, index):
        track_value = index
        self.gacha.set_targeted_weapon(track_value)

    def single_pull(self):
        result = self.gacha.pull()
        self.display_result(result)

    def ten_pull(self):
        results = self.gacha.multi_pull(10)
        for result in results:
            self.display_result(result)

    def display_result(self, result):
        item, rarity = result
        star = "★" * (5 if rarity in [0, 1] else 4 if rarity in [2, 3] else 3)

        self.total_pulls += 1

        # 使用HTML标签设置颜色
        if rarity in [0, 1]:
            color_html = '<font color="#e5bf00">'
            # 打印当前总抽数
            self.result_text.append(f"{self.total_pulls}抽: {color_html}{star} {item}</font>")
            # 重置计数器
            self.current_count = 1
        elif rarity in [2, 3]:
            color_html = '<font color="#875ec0">'
            self.result_text.append(f"{self.current_count}抽: {color_html}{star} {item}</font>")
            self.current_count += 1
        else:
            color_html = '<font color="#3469c6">'
            self.result_text.append(f"{self.current_count}抽: {color_html}{star} {item}</font>")
            self.current_count += 1


if __name__ == "__main__":
    app = QApplication(sys.argv)

    # 创建6个窗口实例
    window1 = GenshinGachaWindow("100452690")
    window2 = GenshinGachaWindow("119420002")
    window3 = GenshinGachaWindow("300000000")
    window4 = GenshinGachaWindow("208938424")
    window5 = GenshinGachaWindow("289933163")
    window6 = GenshinGachaWindow("189933163")

    window1.show()
    window2.show()
    window3.show()
    window4.show()
    window5.show()
    window6.show()

    sys.exit(app.exec_())

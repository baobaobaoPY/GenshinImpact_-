import ctypes
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QPushButton, QLabel, QComboBox, QTextEdit, \
    QMessageBox


class GenshinGachaWindow(QMainWindow):
    def __init__(self, di):
        super().__init__()
        self.di = di
        self.init_ui()
        self.total_pulls = 0
        self.current_count = 1
        # 初始化DLL接口
        self.init_dll()

    def init_dll(self):
        """初始化动态链接库接口"""
        try:
            # 加载DLL
            current_dir = __file__.rsplit('\\', 1)[0]
            dll_path = current_dir + '\\GachaSystem.dll'
            self.gacha_dll = ctypes.CDLL(dll_path)

            # 设置函数原型
            # 创建抽卡系统
            self.gacha_dll.CreateGachaSystem.argtypes = []
            self.gacha_dll.CreateGachaSystem.restype = ctypes.c_void_p

            # 销毁抽卡系统
            self.gacha_dll.DestroyGachaSystem.argtypes = [ctypes.c_void_p]
            self.gacha_dll.DestroyGachaSystem.restype = None

            # 设置定轨
            self.gacha_dll.SetTargetedWeapon.argtypes = [ctypes.c_void_p, ctypes.c_int]
            self.gacha_dll.SetTargetedWeapon.restype = None

            # 单次抽卡
            self.gacha_dll.DoPull.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
            self.gacha_dll.DoPull.restype = None

            # 批量抽卡
            self.gacha_dll.MultiPull.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
            self.gacha_dll.MultiPull.restype = ctypes.c_int

            # 创建系统实例
            self.gacha_system = self.gacha_dll.CreateGachaSystem()

        except Exception as e:
            self.show_error("DLL加载失败", f"无法加载抽卡系统DLL：{str(e)}")

    def show_error(self, title, message):
        """显示错误对话框"""
        QMessageBox.critical(self, title, message)

    def init_ui(self):
        """初始化UI界面"""
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
        self.track_combo.addItem("希诺宁")
        self.track_combo.addItem("温迪")
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
        """处理定轨选择变化"""
        self.gacha_dll.SetTargetedWeapon(self.gacha_system, index)

    def single_pull(self):
        """执行单次抽卡"""
        item_name = ctypes.create_string_buffer(256)
        rarity = ctypes.c_int()
        self.gacha_dll.DoPull(self.gacha_system, item_name, ctypes.byref(rarity))
        self.display_result((item_name.value.decode(), rarity.value))

    def ten_pull(self):
        """执行十次抽卡"""
        buffer_size = 1024 * 100  # 预留足够大的缓冲区
        buffer = ctypes.create_string_buffer(buffer_size)

        count = self.gacha_dll.MultiPull(self.gacha_system, 10, buffer, buffer_size)
        if count > 0:
            results = buffer.value.decode().split('|')
            for result in results[:count]:
                item, rarity = result.split(',')
                self.display_result((item, int(rarity)))

    def display_result(self, result):
        """显示抽卡结果"""
        item, rarity = result
        star = "★" * (5 if rarity in [0, 1] else 4 if rarity in [2, 3] else 3)

        self.total_pulls += 1

        # 使用HTML标签设置颜色
        if rarity in [0, 1]:
            color_html = '<font color="#e5bf00">'
            self.result_text.append(f"{self.total_pulls}抽: {color_html}{star} {item}</font>")
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
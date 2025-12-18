# EdgeSense 六合一传感器控制台

一个基于 Qt Widgets 的嵌入式可视化面板，集中展示并控制多种传感器与执行器。界面采用深色卡片式设计，适合直接运行在开发板的全屏环境中。

## 功能概览
- **DHT11 高精度温湿度**：后台线程定时读取 `/dev/dht11`，实时刷新温度与湿度数值，并驱动折线图绘制。【F:mainwindow.cpp†L29-L130】【F:Dht11/dht11thread.cpp†L41-L93】
- **AP3216 三合一环境光**：线程读取红外、环境光（ALS）和接近（PS）数据，结果显示在独立卡片上，可随时开始/停止监测。【F:mainwindow.cpp†L31-L93】【F:Ap3216/ap3216thread.cpp†L41-L89】
- **ADXL345 三轴加速度**：线程采集 X/Y/Z 三轴数据，利用 LCD 数码管组件直观展示。按钮可切换实时采样。【F:mainwindow.cpp†L33-L96】【F:Adxl345/adxl345thread.cpp†L41-L89】
- **单通道 ADC/电压**：通过 `/sys/bus/iio/devices/iio:device0/in_voltage0_raw` 获取原始 ADC 值，换算成电压（mV）并显示。采样周期 200 ms，可随时启动/暂停。【F:mainwindow.cpp†L115-L159】
- **SK9822 多彩 LED**：写入 `/dev/sk9822` 控制三色 LED，实现关闭、白色与七色预设。颜色下拉框与 UI 绑定，切换即生效。【F:mainwindow.cpp†L59-L93】【F:mainwindow.cpp†L160-L208】
- **蜂鸣器 (Beep)**：通过 sysfs GPIO 导出/收回方式驱动蜂鸣器，按钮样式体现当前状态。提供防抖逻辑，保障打开/关闭对应引脚操作。【F:mainwindow.cpp†L34-L58】【F:mainwindow.cpp†L160-L208】
- **温湿度折线图**：基于 `QCustomPlot`，可勾选显示/隐藏温度或湿度曲线，支持拖拽、缩放与图例。刷新周期 200 ms，自动滑动窗口。【F:mainwindow.cpp†L95-L149】【F:mainwindow.cpp†L209-L261】【F:mainwindow.cpp†L262-L302】

## 全新 UI 设计
- **深色主题与柔和渐变**：背景使用 `#0b1220` 到 `#111827` 的渐变，搭配高对比度文字和多彩强调色，兼顾夜间可读性与硬件屏幕功耗。界面在 `mainwindow.ui` 中定义，开箱即全屏显示。【F:mainwindow.ui†L1-L120】【F:mainwindow.cpp†L11-L63】
- **卡片式信息分区**：各传感器/执行器独立卡片，采用描边+圆角视觉层级，并为按钮配置差异化颜色（温湿度、环境光、加速度、ADC 采集和 LED/蜂鸣器控制）。【F:mainwindow.ui†L89-L215】【F:mainwindow.ui†L351-L1807】
- **顶部信息条**：标题改为 “EdgeSense · 六合一传感器中枢”，附带“实时监测·触控控制·即时反馈”副标题与右上角关闭键，强化设备品牌化和定位。【F:mainwindow.ui†L1810-L1866】
- **交互提示与配色**：勾选框、数码管、曲线颜色与按钮配色保持一致，便于快速对应功能；下拉框与图标按钮增加圆角与悬停反馈，优化触控体验。【F:mainwindow.ui†L89-L215】【F:mainwindow.ui†L880-L1546】

## 运行与调试
1. 使用 Qt 5/6（包含 Widgets 模块）打开或编译 `NewQSensor.pro`，项目会链接 `qcustomplot` 组件。
2. 部署到目标板后直接运行可执行文件，默认全屏显示；如需窗口模式，可在 `MainWindow` 构造函数中移除 `showFullScreen()` 调用。【F:mainwindow.cpp†L11-L65】
3. 运行前确保内核已提供对应设备节点与权限：`/dev/dht11`、`/dev/ap3216c`、`/dev/adxl345`、`/dev/sk9822` 以及 ADC sysfs 路径。
4. 若需调整折线图刷新频率或窗口长度，可修改 `timer_plot->start(200)` 和 `setRange` 相关逻辑。【F:mainwindow.cpp†L32-L149】【F:mainwindow.cpp†L243-L302】

## 交互流程速览
- 点击各卡片的“开始监测”/“停止监测”以切换传感器线程或定时器；按钮颜色体现功能分区，状态文字提示当前模式。【F:mainwindow.cpp†L65-L159】【F:mainwindow.ui†L720-L1780】
- 使用“温度曲线 / 湿度曲线”复选框控制折线图的显示层；拖拽图表可平移，滚轮可缩放。默认自动滚动显示最近数据。【F:mainwindow.cpp†L95-L149】【F:mainwindow.ui†L351-L455】
- 下拉选择 LED 颜色或点击蜂鸣器图标按钮即可即时控制执行器；控制逻辑通过文件描述符和 sysfs GPIO 实现。配置文件路径与颜色预设见 `mainwindow.cpp` 顶部常量定义。【F:mainwindow.cpp†L15-L66】【F:mainwindow.cpp†L160-L208】

# Time-Aware Stream Reservation Mechanism under Distributed Model for TSN

此份code模擬了網路中的各項元件: Switch, EndDevice, Flow, Packet，並提供"類同步"(device間的時間相差最多0.01μs)的方案在網路中傳遞packet. 其中，實作了Asynchronous Traffic Shaping (ATS)及論文中提出的TDMA-based的stream reservation過程. ATS的per-hop delay bound計算為實作IEEE 802.1Qcr V-2&V-9的數學分析公式，因此此自製模擬器不會對delay bound及stream reservation造成任何影響。  

另外，也有實作packet的傳遞及計算其delay，delay的誤差在0.01μs內，然而並未進行嚴謹的correctness驗證，因此論文中不採計兩種方法的delay影響。

## 環境
Windows 10 64bit   
gcc version 8.1.0   
Python 2.7.14   

## 編譯
```sh
g++ -std=c++11 -o StreamReserve.exe *.cc *.cpp
```

## 產生結果圖
1. 於Evaluation資料夾中執行:
```sh
python Evaluation.py
```
2. 將產生Schedulability及Computation time的.dat檔於各自的資料夾中，進入資料夾執行.plt檔

## Documentation
接下來將簡介此程式，及各部件如何運作
### 實作組件
1. Switch:  
TSN交換機，將接收到的packet傳送至相對應的port  
2. SWPort:  
TSN交換機的port，負責接收及發送packet。其中，ATS及TDMA-based方法皆實作在此組件中。  
3. EndDevice:  
終端裝置，負責發送及接收各個flow。  
4. EDPort:  
終端裝置的port，負責接收及發送packet。  
5. Flow:  
抽象化application的flow，不會在網路中傳遞，但會於執行時根據設定的flow attributes(e.g. packet size, period, priority, etc.)傳送packet保留或傳輸。  
6. Packet:  
網路封包，實際於網路中傳遞的實體。由Flow於EndDevice中開始傳輸，經過路徑上的Switch，最後到目的地EndDevice.  
7. Utility:  
實作各除網路組件外的公用function。  

### 原理
網路中各裝置為平行運作，然此程式並未採用thread的方式平行運作各個device(Switch & EndDevice)。此模擬器在Switch & EndDevice中實作 **run()** 這個function，當call一次 **run()** 代表經過0.01μs，因此在執行過程中可以視為幾乎同步。

### Switch
```C
// 建立一個Switch及分配其ID
Switch sw = new Switch(int ID);

// 填寫Routing Table
sw.routing_table[EndDevice_ID] = port_num; // routing_table為map，對應EndDevice的ID -> Switch的port_num
```

### EndDevice
```C
// 建立一個EndDevice及分配其ID
EndDevice ed = new EndDevice(int ID);
```

### Utility
```C
// 建立Utility實體
Utility utility = new Utility();

// 連接兩個裝置(Switch & Switch)，裝置的port將自動生成且port_num累加
utility.connectToSwitch(Switch *sw1, Switch *sw2);

// 連接兩個裝置(Switch & EndDevice)，裝置的port將自動生成且port_num累加
utility.connectToSwitch(Switch *sw, EndDevice *ed);

// 填寫Routing Table至一組連接好的Switch及EndDevice。
/*
 * Warning, 此懶人作法因為模擬器的"偽同步"實作方法，所以有可能在某些
 * topology產生bug導致reservation過程無法完成(packet來回的路徑不同)
 * 此時必須自行填寫各Switch的Routing Table (參考上面Switch)
 */
utility.broadcastEndDevice(std::vector<Switch*> sw, std::vector<EndDevice*> ed);

// 建立TSN flow
utility.setupTSN(Flow *TSN, double period, int packet_size, int source, int destination, int start_time);
utility.setupTSN(Flow *TSN, double period, double deadline, int packet_size, int source, int destination, int start_time);

// 保留TSN flow於連接好的網路(std::vector<Switch*> sw, std::vector<EndDevice*> ed)中
utility.reserveTSN(Flow *TSN, std::vector<Switch*> sw, std::vector<EndDevice*> ed);
```

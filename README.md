Simple 3d renderer that works faster than unity. Test outcomes are below. 

Both tested in same PC(used 3080 as GPU)
-
-
-
-
-
-
-
-
| (Polygons) | TheRenderer (FPS) | Unity (FPS) | difference |
| :--- | :---: | :---: | :---: |
| **230.4 K** | **2850** | 450 | **~6.3x Faster** |
| **576.0 K** | **3750** | 650 | **~5.7x Faster** |
| **11.52 M** | **1065** | 200 | **~5.3x Faster** |
| **17.28 M** | **750** | 160 | **~4.7x Faster** |
| **23.04 M** | **570** | 120 | **~4.7x Faster** |
| **34.56 M** | **395** | 95  | **~4.1x Faster** |
| **46.08 M** | **300** | 70  | **~4.3x Faster** |
| **69.12 M** | **200** | 50  | **~4.0x Faster** |
| **115.2 M** | **120** | 32  | **~3.7x Faster** |
| **172.8 M** | **80** | 22  | **~3.6x Faster** |
| **230.4 M** | **60** | 15  | **~4.0x Faster** |
| **288.0 M** | **50** | 12  | **~4.1x Faster** |
| **345.6 M** | **42** | 10  | **~4.2x Faster** |
| **460.8 M** | **30** | 8   | **~3.7x Faster** |
| **691.2 M** | **22** | 5   | **~4.4x Faster** |
| **921.6 M** | **16** | 4   | **~4.0x Faster** |
| **1.152 B** | **13** | 2   | **~6.5x Faster** |
| **1.728 B** | **9** | Crash/N/A | - |
| **2.304 B** | **7** | Crash/N/A | - |
| **3.456 B** | **4** | Crash/N/A | - |


<img width="1600" height="800" alt="image" src="https://github.com/user-attachments/assets/755956d2-9d49-4faa-a467-9f986c4dea4c" />
<img width="1000" height="600" alt="image" src="https://github.com/user-attachments/assets/fe865a4d-f430-43f9-b878-81d5c4c32a65" />


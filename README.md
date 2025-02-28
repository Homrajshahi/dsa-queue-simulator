## 🚦 DSA Queue Simulator: A Stylish Traffic Junction Adventure

Welcome to the **DSA Queue Simulator**—an elegant, queue-driven traffic junction simulator designed for a dynamic four-road intersection (A, B, C, D), each boasting three lanes. Crafted for **Assignment #1** in *Data Structures and Algorithms (COMP202)* at **Kathmandu University’s Department of Computer Science and Engineering**, this project harnesses the power of **circular queues** and **SDL2** to deliver a visually stunning, real-time traffic management experience. ✨

---

## 🌟 Overview
This simulator orchestrates traffic flow at a bustling junction, ensuring fairness under normal conditions and prioritizing **Road A’s Lane 2 (AL2)** when congestion spikes above **10 vehicles** (until under **5**). With **SDL2’s magic**, roads, traffic lights *(t1, t2, t3, t4)*, and vehicles—**blue for regular cars, red/orange for emergencies**—are rendered at a crisp **60 FPS**. Vehicles move, pause at zebra crossings, and strive to navigate turns *(e.g., Lane 1 [L1] turning left to Lane 3 [L3] on the target road).* 

🚧 However, **L1 turning logic** and **AL2 priority logic** remain unresolved, alongside minor hiccups like **vehicle jittering, premature dequeuing, and light lag**. These challenges are detailed in the report and slated for future refinements—stay tuned for the polished masterpiece! 🚗💨

---

## 🚀 Features

✅ **Queue-Powered Precision:** Uses **12 circular queues** (3 lanes per road × 4 roads) for **lightning-fast O(1) enqueue/dequeue operations**.

✅ **Stunning Visualization:** Powered by **SDL2**, crafting an immersive **real-time traffic experience**.

✅ **Dynamic Traffic Lights:** Cycles **every 7 seconds**, smoothly controlling traffic flow.

✅ **Diverse Vehicle Types:** **90% regular cars**, **10% emergency vehicles**, speeds ranging from **0.2 to 0.8** units.

✅ **File-Based Harmony:** **vehicles.txt** integration allows **fluid, real-time updates**.

---

## ⚠️ Current Limitations
💥 **Turning Logic:** L1 vehicles *(e.g., AL1, BL1)* fail to complete left turns to L3 *(e.g., AL1 to BL3)*, stalling at zebra crossings.

💥 **Priority Logic:** AL2 **does not activate priority mode** when congestion exceeds **10 vehicles**.

💥 **Minor Glitches:**
   - 🚗 Vehicle **jittering/collisions** at zebra crossings.
   - 🚗 Vehicles **vanishing too soon** post-turn.
   - 🚦 **Traffic light transitions** experiencing slight lag.

These will be resolved in **future updates**, ensuring an even more **polished and refined experience**! 🔧

---

## 🛠 How to Run the Program

💡 **Step 1: Clone the Repository**  
```bash
git clone https://github.com/[YourGitHubUsername]/dsa-queue-simulator.git
cd dsa-queue-simulator/bin
```

💡 **Step 2: Install Dependencies**  
For **Ubuntu/Debian**:
```bash
sudo apt-get update && sudo apt-get install libsdl2-dev
```
For **macOS (Homebrew)**:
```bash
brew install sdl2
```

💡 **Step 3: Compile the Code**  
```bash
make
```

💡 **Step 4: Run the Simulator**  
```bash
./simulator
```

💡 **Step 5: Simulate Vehicle Generation**  
Edit or create `vehicles.txt` in the `bin/` directory:
```text
vehicleId,type,startDirection,endDirection,startLane,endLane,x,y,speed,turnAngle,turning,progress,waitTime,passedIntersection
1,0,0,1,0,2,360.0,460.0,0.5,0.0,0,0.0,0,0
```

💡 **Optional: Traffic Generator**  
A future **traffic_generator.c** will automate vehicle creation, but for now, manual updates to `vehicles.txt` suffice!

---

## 🎥 Demonstration
📹 **A 30-second GIF** is available in `docs/demo.gif`, showcasing:
- 🚗 **Real-time vehicle movement & traffic light transitions**.
- 🛑 **Zebra crossing halts and attempted L1 turns**.
- 🔍 **Minor bugs visible in simulation (jittering, premature dequeuing).**

📌 *This preview captures the current state; enhancements will follow soon!* 🚀

---

## 📂 Project Structure
📁 `simulator.c` → **SDL2 visualization, queue management, traffic logic.**
📁 `queue.c/queue.h` → **Circular queues for lane/vehicle management.**
📁 `bin/` → **Executables & vehicle data (`vehicles.txt`).**
📁 `Makefile` → **Effortless SDL2 compilation.**
📁 `docs/` → **Demo GIF + assignment report (`report.pdf`).**

---

## 🛤️ Development Journey
🔹 **25+ commits** shaping the evolution from queue fundamentals to **real-time traffic orchestration**.
🔹 **Challenges & debugging efforts** meticulously documented in `docs/report.pdf`.

---

## 📚 References
📖 **SDL2 Documentation** - [SDL2/FrontPage](https://wiki.libsdl.org/SDL2/FrontPage)  
📖 **Introduction to Algorithms** - Cormen et al. *(Queue structures & efficiency)*  
📖 **Kathmandu University COMP202 Materials** - *Queue-based traffic solutions*  
📖 **Federal Highway Administration** - *Traffic Flow Theory & real-world principles*  

---

## 🌠 Future Vision
🔹 **Resolve Turning Logic:** Improve progress updates & turn radius for **seamless L1 turns**.  
🔹 **Implement Priority Logic:** Debug `isPriorityLaneActive` & `canProceedThroughIntersection`.  
🔹 **Fix Minor Glitches:** Address **jittering**, **dequeuing issues**, & **traffic light lag**.  
🔹 **Expand Features:** Introduce **real-time traffic generator**, additional vehicle types, & **complex intersections**.  

---

🚀 **Join the journey!** Run, explore, and contribute to the **DSA Queue Simulator**, celebrating its **current elegance** while anticipating **limitless potential!** 🌟

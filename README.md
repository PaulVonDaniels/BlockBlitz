# BlockBlitz
<img width="499" height="763" alt="изображение" src="https://github.com/user-attachments/assets/42850b0e-7906-4c0c-bac0-2b0ce3fe74fb" /> \
Qt/C++ Project. The following is a write-up handed in with the assignment.


# Registration window
Greets players with a stylish authorization window where you can log in with an existing username or register a new account.  The
intuitive interface with input verification and support for QSS styles provides a convenient start to the game.  
Registration saves data locally, allowing players to go back to their achievements. \
<img width="433" height="310" alt="изображение" src="https://github.com/user-attachments/assets/012aa7b0-d6ee-4981-bb59-374565b73a4f" />


# Description
**BlockBlitz** offers three exciting game modes: \
**Classic**: Traditional Tetris for a relaxing game.  
**Marathon**: An accelerating pace with increasing difficulty.  
**Challenge**: Clear the pre-filled fields in a limited time 


<img width="367" height="370" alt="изображение" src="https://github.com/user-attachments/assets/ed35354c-6992-4793-bf94-218a5b38191d" />


# Features
1. Three modes: Classic, Marathon and Challenge.
2. Intuitive controls: Move, Spin, Instant drop.  
3. Combo system and quick action bonuses.
4. Authorization and registration of users.
5. Shadow the shape for precise placement.
6. Customizable interface via QSS.
7. Smooth gameplay without lags.

# Requirements
**Qt 5 or 6** (the `widgets` module)  
Compiler with support for **C++11**  
Development environment (recommended by Qt Creator)


# Testing

During several stages of creating my program and after I made significant changes that could affect the user experience, I collected feedback on how I could improve my program by adding, removing, or changing certain elements. One of the feedback I received at the end of my program was about how a user could learn to play if they had never played Tetris before. At that time, my game didn't have anything on how to play, so later I'll add a help screen that can be accessed from the Start menu by pressing "h". This menu shows all possible keys that the user can press while working with the program, and a few words describing what this key does. On the other side of the screen there is a description of the Tetris game and its purpose. There is also a small table below it showing which actions are awarded points and how many points are awarded for each action. After working with this menu, I think it has added a lot to the program and would be a great help for beginners in Tetris.





	██████╗  █████╗ ██╗   ██╗██╗
	██╔══██╗██╔══██╗██║   ██║██║
	██████╔╝███████║██║   ██║██║
	██╔═══╝ ██╔══██║╚██╗ ██╔╝██║
	██║     ██║  ██║ ╚████╔╝ ███████╗
	╚═╝     ╚═╝  ╚═╝  ╚═══╝  ╚══════╝

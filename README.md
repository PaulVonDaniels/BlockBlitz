# BlockBlitz


https://user-images.githubusercontent.com/70239160/154780145-1ba126a1-fa6a-4efc-81fa-4fa488146f82.mp4

Major C++ Project. The following is a write-up handed in with the assignment.


# Description

**BlockBlitz** offers three exciting game modes: \
**Classic**: Traditional Tetris for a relaxing game.  
**Marathon**: An accelerating pace with increasing difficulty.  
**Challenge**: Clear the pre-filled fields in a limited time

# Особенности

1. Три режима: Classic, Marathon и Challenge.  
2. Интуитивное управление: перемещение, вращение, мгновенное падение.  
3. Система комбо и бонусы за быстрые действия.
4. Авторизация и регистрация пользователей.
5. Тень фигуры для точного размещения.
6. Настраиваемый интерфейс через QSS.
7. Плавный геймплей без лагов.

# Требования
**Qt 5 или 6** (модуль `widgets`)  
Компилятор с поддержкой **C++11**  
Среда разработки (рекомендуется Qt Creator)  


# Testing

During several stages of creating my program and after I made significant changes that could affect the user experience, I collected feedback on how I could improve my program by adding, removing, or changing certain elements. One of the feedback I received at the end of my program was about how a user could learn to play if they had never played Tetris before. At that time, my game didn't have anything on how to play, so later I'll add a help screen that can be accessed from the Start menu by pressing "h". This menu shows all possible keys that the user can press while working with the program, and a few words describing what this key does. On the other side of the screen there is a description of the Tetris game and its purpose. There is also a small table below it showing which actions are awarded points and how many points are awarded for each action. After working with this menu, I think it has added a lot to the program and would be a great help for beginners in Tetris.

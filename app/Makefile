all: vault_hw.exe

main.o:
	g++ -std=c++17 -Wall -Wextra -I. -c src/main.cpp -o main.o

Category.o:
	g++ -std=c++17 -Wall -Wextra -I. -c src/Category.cpp -o Category.o

Resistor.o:
	g++ -std=c++17 -Wall -Wextra -I. -c src/Resistor.cpp -o Resistor.o

Transistor.o:
	g++ -std=c++17 -Wall -Wextra -I. -c src/Transistor.cpp -o Transistor.o

Diode.o:
	g++ -std=c++17 -Wall -Wextra -I. -c src/Diode.cpp -o Diode.o

CustomCategory.o:
	g++ -std=c++17 -Wall -Wextra -I. -c src/CustomCategory.cpp -o CustomCategory.o

Component.o:
	g++ -std=c++17 -Wall -Wextra -I. -c src/Component.cpp -o Component.o

UsedComponent.o:
	g++ -std=c++17 -Wall -Wextra -I. -c src/UsedComponent.cpp -o UsedComponent.o

Project.o:
	g++ -std=c++17 -Wall -Wextra -I. -c src/Project.cpp -o Project.o

Inventory.o:
	g++ -std=c++17 -Wall -Wextra -I. -c src/Inventory.cpp -o Inventory.o

vault_hw.exe: main.o Category.o Resistor.o Transistor.o Diode.o CustomCategory.o Component.o UsedComponent.o Project.o Inventory.o
	g++ -std=c++17 -Wall -Wextra main.o Category.o Resistor.o Transistor.o Diode.o CustomCategory.o Component.o UsedComponent.o Project.o Inventory.o -o vault_hw.exe

run:
	./vault_hw.exe

clean:
	del *.exe
	del *.o
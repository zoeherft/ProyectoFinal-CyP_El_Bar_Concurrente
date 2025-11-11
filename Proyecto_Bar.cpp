/*En un bar muy concurrido, los clientes llegan constantemente para hacer sus pedidos de bebidas; cada clientedebe esperar a que uno de los baristas tome su orden, la prepare con cuidado y la deje lista en la barra, donde los meseros se encargan de recogerla y entregarla al cliente correspondiente; sin embargo, el bar tiene espacio limitado en su barra y en la mesa de pedidos, lo que obliga a organizar el flujo de trabajo: si hay demasiados pedidos acumulados, los clientes deben esperar antes de ordenar, y si no hay bebidas listas, los meseros se ven obligados a esperar hasta que los baristas terminen; de este modo, el problema consiste en diseñar un sistema concurrente/paralelo con hilos que modele este escenario, coordinando a clientes, baristas y meseros mediante mecanismos ya sea con
memoria compartida  o bien con paso de mensajes (elegir sólo una forma de solución), garantizando la correctitud en la ejecución del programa que se implemente para solucionar el problema.

Sugerencias:
a) Los Clientes son hilos productores que llegan y hacen
pedidos.
    Los Baristas son hilos procesadores preparan las
bebidas. 
    Los Meseros son hilos consumidores entregan las
bebidas listas a los clientes.

b) Utilizar un buffer circular para almacenar los pedidos
     Los baristas toman pedidos del buffer circular y los procesan
     Los meseros toman los pedidos listos de otro buffer y los entregan

c) El número de clientes, baristas y meseros es variable, por ejemplo: 5 clientes, 2 baristas y 2 meseros
     El tiempo de preparación de las bebidas se puede simular con sleeps
     El tamaño de los buffers/colas y de los pedidos debe ser limitado para saber cuando estan llenos o vacíos;  por ejemplo: 60 pedidos, 
     un buffer circular de tamaño 8 para los pedidos almacenados, un buffer de tamaño 8 para los pedidos procesados*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <cstdlib>   // rand(), srand(), exit()
#include <ctime>     // time()
#include <array>     // std::array
#include <chrono>    // sleep_for
#include <vector>

using namespace std;

mutex candado;
condition_variable vc;

// Clase bebidas
class bebidas
{
public:
    string nombre;  // nombre de la bebida
    int cliente;    // id del cliente
    int borrado;    // 0 = ocupado, 1 = libre

public:
    bebidas() // Las bebidas se inicializarán borradas
    {
        this->cliente = -1;
        this->nombre = "No Asignado";
        this->borrado = 1;
    }

    void asigCont(int cliente) // Abreviacion de asignar contenido
    {
        this->cliente = cliente;
        this->borrado = 0;

        int aleatorio = rand() % 6;  // 0..5

        switch (aleatorio) // Se elegirá el nombre de la bebida de forma aleatoria
        {
        case 0:
            this->nombre = "Cuba";
            break;
        case 1:
            this->nombre = "Paloma";
            break;
        case 2:
            this->nombre = "Piña Colada";
            break;
        case 3:
            this->nombre = "Mojito";
            break;
        case 4:
            this->nombre = "Margarita";
            break;
        case 5:
            this->nombre = "Manhattan";
            break;
        default:
            cout << "Considera poner un puesto de tacos" << endl;
            exit(1);
            break;
        }
    }
};


// Cola circular

class colaCircular
{
public:
    array<bebidas, 8> pedidos;
    int inicio, fin;
    bool vacia, llena;

public:
    colaCircular()
    {
        inicio = 0; // posición del siguiente elemento a eliminar
        fin = 0;    // posición del siguiente elemento a insertar

        for (int i = 0; i < 8; i++)
        {
            pedidos[i] = bebidas();
        }

        vacia = true;
        llena = false;
    }

    void esVacia() // Actualiza la bandera vacia
    {
        vacia = true;
        for (int i = 0; i < 8; i++)
        {
            if (pedidos[i].borrado == 0) // hay al menos un lugar ocupado
            {
                vacia = false;
                break;
            }
        }
    }

    void esLlena() // Actualiza la bandera llena
    {
        llena = true;
        for (int i = 0; i < 8; i++)
        {
            if (pedidos[i].borrado == 1) // hay al menos un lugar libre
            {
                llena = false;
                break;
            }
        }
    }

    // Agregar un pedido completo (bebida ya asignada)
    void agregarPedido(const bebidas &nuevo)
    {
        unique_lock<mutex> lk(candado);

        esLlena();
        while (llena) // si la cola está llena, espera
        {
            vc.wait(lk);
            esLlena();
        }

        pedidos[fin] = nuevo;      // copiamos la bebida
        pedidos[fin].borrado = 0;  // ocupada

        fin = (fin + 1) % 8;       // avanzar fin de forma circular

        esVacia();
        vc.notify_all();
    }

    // Eliminar y devolver un pedido
    bebidas eliminarPedido()
    {
        unique_lock<mutex> lk(candado);

        esVacia();
        while (vacia) // si la cola está vacía, espera
        {
            vc.wait(lk);
            esVacia();
        }

        bebidas extraida = pedidos[inicio];  // copiamos el elemento

        pedidos[inicio].borrado = 1;  // marcar como libre

        inicio = (inicio + 1) % 8;    // avanzar inicio de forma circular

        esLlena();
        vc.notify_all();

        return extraida;
    }
};


// Buffers globales

colaCircular colaPedidos;  // pedidos que hacen los clientes
colaCircular colaListos;   // pedidos ya preparados por baristas


// Parámetros del sistema

const int num_clientes = 5;
const int num_baristas = 2;
const int num_meseros  = 2;

const int pedidos_por_cliente = 12;  // 5 * 12 = 60
const int total_pedidos = num_clientes * pedidos_por_cliente;


// Función de ayuda para simular tiempo

void dormir_ms(int min_ms, int max_ms)
{
    int rango = max_ms - min_ms + 1;
    int t = min_ms + (rand() % rango);
    this_thread::sleep_for(chrono::milliseconds(t));
}


// Hilo: Cliente (productor)

void cliente(int id_cliente)
{
    for (int i = 0; i < pedidos_por_cliente; i++)
    {
        bebidas nuevo;
        nuevo.asigCont(id_cliente);

        cout << "[Cliente " << id_cliente << "] Hace pedido "
             << i << " (" << nuevo.nombre << ")\n";

        colaPedidos.agregarPedido(nuevo);

        dormir_ms(100, 400); // tiempo entre pedidos
    }
}


// Hilo: Barista (procesador)

void barista(int id_barista, int cuantos)
{
    for (int i = 0; i < cuantos; i++)
    {
        bebidas ped = colaPedidos.eliminarPedido();

        cout << "  [Barista " << id_barista << "] Preparando bebida de cliente "
             << ped.cliente << " (" << ped.nombre << ")\n";

        dormir_ms(300, 800); // tiempo de preparación

        cout << "  [Barista " << id_barista << "] Bebida lista para cliente "
             << ped.cliente << " (" << ped.nombre << ")\n";

        // El mismo pedido pasa ahora a la cola de listos
        colaListos.agregarPedido(ped);
    }
}


// Hilo: Mesero (consumidor)

void mesero(int id_mesero, int cuantos)
{
    for (int i = 0; i < cuantos; i++)
    {
        bebidas listo = colaListos.eliminarPedido();

        cout << "    [Mesero " << id_mesero << "] Entregando bebida a cliente "
             << listo.cliente << " (" << listo.nombre << ")\n";

        dormir_ms(200, 600); // tiempo de entrega
    }
}


// main

int main()
{
    srand((unsigned)time(NULL)); // inicializar aleatorio UNA sola vez

    cout << "Simulacion del bar concurrencte/paralelo\n";
    cout << "Total de pedidos: " << total_pedidos << "\n\n";

    // --- Crear hilos de clientes ---
    vector<thread> hilos_clientes;
    for (int i = 0; i < num_clientes; i++)
    {
        hilos_clientes.emplace_back(cliente, i);
    }

    // Repartir pedidos entre baristas
    vector<int> pedidos_por_barista(num_baristas, total_pedidos / num_baristas);
    int resto = total_pedidos % num_baristas;
    for (int i = 0; i < resto; i++)
        pedidos_por_barista[i]++;

    // --- Crear hilos de baristas ---
    vector<thread> hilos_baristas;
    for (int i = 0; i < num_baristas; i++)
    {
        hilos_baristas.emplace_back(barista, i, pedidos_por_barista[i]);
    }

    // Repartir pedidos entre meseros
    vector<int> pedidos_por_mesero(num_meseros, total_pedidos / num_meseros);
    resto = total_pedidos % num_meseros;
    for (int i = 0; i < resto; i++)
        pedidos_por_mesero[i]++;

    // --- Crear hilos de meseros ---
    vector<thread> hilos_meseros;
    for (int i = 0; i < num_meseros; i++)
    {
        hilos_meseros.emplace_back(mesero, i, pedidos_por_mesero[i]);
    }

    // Esperar a que terminen todos los hilos
    for (auto &t : hilos_clientes) t.join();
    for (auto &t : hilos_baristas) t.join();
    for (auto &t : hilos_meseros) t.join();

    cout << "\nTodos los pedidos han sido atendidos. Cierre del bar.\n";
    return 0;
}

# 🍸 Simulación Concurrente de un Bar — Programación CyP

Este proyecto implementa un **sistema concurrente en C++** que modela el flujo de trabajo dentro de un bar muy concurrido, utilizando **hilos**, **mutex** y **variables de condición** para coordinar a **clientes**, **baristas** y **meseros**.

## 🧠 Descripción General

En un bar concurrido, los clientes hacen pedidos constantemente, los baristas los preparan y los meseros los entregan.  
Sin embargo, tanto la barra como la mesa de pedidos tienen espacio limitado, por lo que se debe **sincronizar correctamente** el flujo de trabajo para evitar:
- que los clientes saturen la barra con pedidos,
- que los baristas se queden sin trabajo,
- o que los meseros esperen bebidas que aún no están listas.

El sistema usa **memoria compartida** para coordinar a los hilos y simular el comportamiento real del proceso.

---

## ⚙️ Estructura del Sistema

### 1. Clientes (Productores)
- Generan pedidos de bebidas de manera aleatoria.
- Colocan sus pedidos en una **cola circular** llamada `colaPedidos`.

### 2. Baristas (Procesadores)
- Toman pedidos de la `colaPedidos`.
- Preparan las bebidas (simulado con `sleep`).
- Colocan las bebidas listas en otra **cola circular** llamada `colaListos`.

### 3. Meseros (Consumidores)
- Toman bebidas de `colaListos`.
- Entregan las bebidas al cliente correspondiente (también con `sleep`).

---

## 🔁 Sincronización

- Se utilizan `std::mutex` y `std::condition_variable` para evitar **condiciones de carrera** y **esperas activas**.
- Cada cola circular tiene un tamaño limitado de **8 elementos**, modelando el espacio reducido del bar.
- Cuando una cola está llena, los hilos productores esperan.
- Cuando está vacía, los hilos consumidores esperan.

---

## 🧩 Clases Principales

| Clase | Función | Descripción |
|--------|----------|-------------|
| `bebidas` | Representa cada pedido | Guarda el nombre de la bebida, el cliente que la pidió y su estado (`borrado`) |
| `colaCircular` | Buffer compartido | Almacena pedidos o bebidas listas de forma circular con control de concurrencia |

---

## 🚀 Ejecución

### 🔧 Compilación
En terminal o CMD, ejecuta:
```bash
g++ -std=c++11 bar_concurrente.cpp -pthread -o bar

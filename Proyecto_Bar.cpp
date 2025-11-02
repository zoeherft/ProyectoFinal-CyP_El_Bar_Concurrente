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
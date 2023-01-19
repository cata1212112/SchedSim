#include "LinkedList.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

typedef struct statistici {
    double cpu_utilization; /// ce procent din timp cpu-ul este folosit
    double average_turnaround_time; /// timpul mediu de finalizare al proceselor (din momentul in care intra in sistem pana se termina)
    double average_waiting_time; /// timpul mediu de asteptare al proceselor (cat timp asteapta fara sa fie executat)
    double average_response_time; /// timpul mediu de raspuns al proceselor (din momentul in care intra in sistem pana sunt executate prima oara)
}statistici;


statistici FirstComeFirstServe(queue procese) {
    queue procese_ready; /// Coada cu procesele gata de exceturie
    init(&procese_ready);
    int time = 0; /// Timpul curent
    proces proces_curent_executat; /// Procesul care e in executie
    proces_curent_executat.pid = -1;
    statistici stats; /// Statiscile algoritmului
    while (1) {
        /// Cand nu mai vine niciun proces, nu execut niciun proces si nu mai am procese gata de executie inseamana ca s-a terminat
        if (proces_curent_executat.pid == -1 && empty(&procese_ready) && empty(&procese)) break;

        /// Adaug in coada cu procele gata de executie procesele ce au ajuns la timpul 'timp'
        if (!empty(&procese)) {
            proces aux;
            while (!empty(&procese)) {
                aux = front(&procese);
                if (aux.arrival == time) {
                    push(&procese_ready, newNode(aux));
                    pop(&procese);
                } else break;
            }
        }

        //// Daca in prezent nu execut niciun proces il iau pe primul ajuns in coada de procese gata de executie
        if (proces_curent_executat.pid == -1 && !empty(&procese_ready)) {
            proces_curent_executat = front(&procese_ready);
            pop(&procese_ready);
            if (proces_curent_executat.response_time == -1) {
                proces_curent_executat.response_time = time - proces_curent_executat.arrival;
            }
            proces_curent_executat.start_time = time;
        }

        //// Daca am un proces in executie atunci il execut
        if (proces_curent_executat.pid != -1) {
            stats.cpu_utilization++;
            proces_curent_executat.remaining--;
            if (proces_curent_executat.remaining == 0) {
                stats.average_waiting_time += proces_curent_executat.start_time - proces_curent_executat.arrival;
//                printf("%d %d\n", stats.average_waiting_time, proces_curent_executat.start_time - proces_curent_executat.arrival);
                stats.average_turnaround_time += time - proces_curent_executat.arrival + 1;
                stats.average_response_time += proces_curent_executat.response_time;
                proces_curent_executat.pid = -1;
            }
        }
        time++;
    }
    stats.average_turnaround_time /= procese.num_procese;
    stats.average_waiting_time /= procese.num_procese;
    stats.cpu_utilization = 100 * stats.cpu_utilization / time;
    stats.average_response_time /= procese.num_procese;
    return stats;
}

statistici ShortestJobFirst(queue procese) {
    queue procese_ready; /// Coada cu procesele gata de exceturie
    init(&procese_ready);
    int time = 0; /// Timpul curent
    proces proces_curent_executat; /// Procesul care e in executie
    proces_curent_executat.pid = -1;
    statistici stats; /// Statiscile algoritmului
    while (1) {
        /// Cand nu mai vine niciun proces, nu execut niciun proces si nu mai am procese gata de executie inseamana ca s-a terminat
        if (proces_curent_executat.pid == -1 && empty(&procese_ready) && empty(&procese)) break;

        /// Adaug in coada cu procele gata de executie procesele ce au ajuns la timpul 'timp'
        if (!empty(&procese)) {
            proces aux;
            while (!empty(&procese)) {
                aux = front(&procese);
                if (aux.arrival == time) {
                    push(&procese_ready, newNode(aux));
                    pop(&procese);
                } else break;
            }
        }


        //// Daca in prezent nu execut niciun proces il iau pe ce lcu timpul de executie cel mai mic din coada de procese gata de executie
        if (proces_curent_executat.pid == -1 && !empty(&procese_ready)) {
            proces_curent_executat = get_min_remaining(&procese_ready);
            delete_pid(&procese_ready, proces_curent_executat.pid);
            proces_curent_executat.start_time = time;
            if (proces_curent_executat.response_time == -1) {
                proces_curent_executat.response_time = time - proces_curent_executat.arrival;
            }
        }

        //// Daca am un proces in executie atunci il execut
        if (proces_curent_executat.pid != -1) {
            stats.cpu_utilization++;
            proces_curent_executat.remaining--;
            if (proces_curent_executat.remaining == 0) {
                stats.average_waiting_time += proces_curent_executat.start_time - proces_curent_executat.arrival;
                stats.average_turnaround_time += time - proces_curent_executat.arrival + 1;
                stats.average_response_time += proces_curent_executat.response_time;
                proces_curent_executat.pid = -1;
            }
        }

        time++;
    }
    stats.average_turnaround_time /= procese.num_procese;
    stats.average_waiting_time /= procese.num_procese;
    stats.cpu_utilization = 100 * stats.cpu_utilization / time;
    stats.average_response_time /= procese.num_procese;
    return stats;
}

statistici ShortestRemainingTimeFirst(queue procese) {
    queue procese_ready; /// Coada cu procesele gata de exceturie
    init(&procese_ready);
    int time = 0; /// Timpul curent
    proces proces_curent_executat; /// Procesul care e in executie
    proces_curent_executat.pid = -1;
    statistici stats; /// Statiscile algoritmului
    while (1) {
        /// Cand nu mai vine niciun proces, nu execut niciun proces si nu mai am procese gata de executie inseamana ca s-a terminat
        if (proces_curent_executat.pid == -1 && empty(&procese_ready) && empty(&procese)) break;

        /// Adaug in coada cu procele gata de executie procesele ce au ajuns la timpul 'timp' si vad procesul cu cel mai mic cpu burst
        proces min_timp;
        min_timp.pid = -1;
        min_timp.remaining = 1e9;

        if (!empty(&procese)) {
            proces aux;
            while (!empty(&procese)) {
                aux = front(&procese);
                if (aux.arrival == time) {
                    if (aux.remaining < min_timp.remaining) {
                        min_timp = aux;
                    }
                    push(&procese_ready, newNode(aux));
                    pop(&procese);
                } else break;
            }
        }

        //// Daca in prezent nu execut niciun proces il iau pe cel cu timpul de executie cel mai mic din coada de procese gata de executie
        if (proces_curent_executat.pid == -1 && !empty(&procese_ready)) {
            proces_curent_executat = get_min_remaining(&procese_ready);
            delete_pid(&procese_ready, proces_curent_executat.pid);
            proces_curent_executat.start_time = time;
            proces_curent_executat.wait_time += proces_curent_executat.start_time - proces_curent_executat.last_active;
            if (proces_curent_executat.response_time == -1) {
                proces_curent_executat.response_time = time - proces_curent_executat.arrival;
            }
        }


        /// Daca a ajuns un proces cu CPU burst mai mic decat procesul ce se executa, iar in caz afirmativ fac context switch
        //// Daca am un proces in executie atunci il execut
        if (proces_curent_executat.pid != -1) {

            if (min_timp.pid != -1) {
                if (min_timp.remaining < proces_curent_executat.remaining) {
                    delete_pid(&procese_ready, min_timp.pid);
                    proces_curent_executat.last_active = time;
                    push(&procese_ready, newNode(proces_curent_executat));
                    proces_curent_executat = min_timp;
                    if (proces_curent_executat.start_time == -1) {
                        proces_curent_executat.start_time = time;
                    }
                    proces_curent_executat.wait_time += time - proces_curent_executat.last_active;
                    if (proces_curent_executat.response_time == -1) {
                        proces_curent_executat.response_time = time - proces_curent_executat.arrival;
                    }
                }
            }
            /// aici e executia propriu zisa
            stats.cpu_utilization++;
            proces_curent_executat.remaining--;
            if (proces_curent_executat.remaining == 0) {
                stats.average_waiting_time += proces_curent_executat.wait_time;
                stats.average_turnaround_time += time - proces_curent_executat.arrival + 1;
                stats.average_response_time += proces_curent_executat.response_time;
                proces_curent_executat.pid = -1;
            }
        }

        time++;
    }
    stats.average_response_time /= procese.num_procese;
    stats.average_waiting_time /= procese.num_procese;
    stats.average_turnaround_time /= procese.num_procese;
    stats.cpu_utilization = 100 * stats.cpu_utilization / time;
    return stats;
}

statistici LongestJobFirst(queue procese) {
    queue procese_ready; /// Coada cu procesele gata de exceturie
    init(&procese_ready);
    int time = 0; /// Timpul curent
    proces proces_curent_executat; /// Procesul care e in executie
    proces_curent_executat.pid = -1;
    statistici stats; /// Statiscile algoritmului
    while (1) {
        /// Cand nu mai vine niciun proces, nu execut niciun proces si nu mai am procese gata de executie inseamana ca s-a terminat
        if (proces_curent_executat.pid == -1 && empty(&procese_ready) && empty(&procese)) break;

        /// Adaug in coada cu procele gata de executie procesele ce au ajuns la timpul 'timp'
        if (!empty(&procese)) {
            proces aux;
            while (!empty(&procese)) {
                aux = front(&procese);
                if (aux.arrival == time) {
                    push(&procese_ready, newNode(aux));
                    pop(&procese);
                } else break;
            }
        }


        //// Daca in prezent nu execut niciun proces il iau pe cel cu timpul de executie cel mai mare din coada de procese gata de executie
        if (proces_curent_executat.pid == -1 && !empty(&procese_ready)) {
            proces_curent_executat = get_max_remaining(&procese_ready);
            delete_pid(&procese_ready, proces_curent_executat.pid);
            proces_curent_executat.start_time = time;
            if (proces_curent_executat.response_time == -1) {
                proces_curent_executat.response_time = time - proces_curent_executat.arrival;
            }
        }
        //// Daca am un proces in executie atunci il execut
        if (proces_curent_executat.pid != -1) {
            stats.cpu_utilization++;
            proces_curent_executat.remaining--;
            if (proces_curent_executat.remaining == 0) {
                stats.average_waiting_time += proces_curent_executat.start_time - proces_curent_executat.arrival;
                stats.average_turnaround_time += time - proces_curent_executat.arrival + 1;
                stats.average_response_time += proces_curent_executat.response_time;
                proces_curent_executat.pid = -1;
            }
        }

        time++;
    }
    stats.average_turnaround_time /= procese.num_procese;
    stats.average_waiting_time /= procese.num_procese;
    stats.cpu_utilization = 100 * stats.cpu_utilization / time;
    stats.average_response_time /= procese.num_procese;
    return stats;
}

statistici LongestRemaningTimeFirst(queue procese) {
    queue procese_ready; /// Coada cu procesele gata de exceturie
    init(&procese_ready);
    int time = 0; /// Timpul curent
    proces proces_curent_executat; /// Procesul care e in executie
    proces_curent_executat.pid = -1;
    statistici stats; /// Statiscile algoritmului
    while (1) {
        /// Cand nu mai vine niciun proces, nu execut niciun proces si nu mai am procese gata de executie inseamana ca s-a terminat
        if (proces_curent_executat.pid == -1 && empty(&procese_ready) && empty(&procese)) break;

        /// Adaug in coada cu procele gata de executie procesele ce au ajuns la timpul 'timp' si vad procesul cu cel mai mare cpu burst

        if (!empty(&procese)) {
            proces aux;
            while (!empty(&procese)) {
                aux = front(&procese);
                if (aux.arrival == time) {
                    push(&procese_ready, newNode(aux));
                    pop(&procese);
                } else break;
            }
        }

        /// La fiecare pas rulez procesul cu timpul de executie ramas cel mai mare
        if (proces_curent_executat.pid != -1) {
            proces_curent_executat.last_active = time;
            push(&procese_ready, newNode(proces_curent_executat));
        }

        if (!empty(&procese_ready)) {
            proces_curent_executat = get_max_remaining(&procese_ready);

            delete_pid(&procese_ready, proces_curent_executat.pid);
            proces_curent_executat.wait_time += time - proces_curent_executat.last_active;
            if (proces_curent_executat.response_time == -1) {
                proces_curent_executat.response_time = time - proces_curent_executat.arrival;
            }

            stats.cpu_utilization++;
            proces_curent_executat.remaining--;
            if (proces_curent_executat.remaining == 0) {
                stats.average_waiting_time += proces_curent_executat.wait_time;
                stats.average_turnaround_time += time - proces_curent_executat.arrival + 1;
                stats.average_response_time += proces_curent_executat.response_time;
                proces_curent_executat.pid = -1;
            }
        }


        time++;
    }
    stats.average_turnaround_time /= procese.num_procese;
    stats.average_waiting_time /= procese.num_procese;
    stats.cpu_utilization = 100 * stats.cpu_utilization / time;
    stats.average_response_time /= procese.num_procese;
    return stats;
}

statistici RoundRobin(queue procese, int time_quantum) {
    queue procese_ready; /// Coada cu procesele gata de exceturie
    init(&procese_ready);
    int time = 0; /// Timpul curent
    proces proces_curent_executat; /// Procesul care e in executie
    proces_curent_executat.pid = -1;
    statistici stats; /// Statiscile algoritmului

    while (1) {
        if (proces_curent_executat.pid == -1 && empty(&procese_ready) && empty(&procese)) break;


        if (!empty(&procese)) {
            proces aux;
            while (!empty(&procese)) {
                aux = front(&procese);
                if (aux.arrival == time) {
                    push(&procese_ready, newNode(aux));
                    pop(&procese);
                } else break;
            }
        }
        //// Daca in prezent nu execut niciun proces il iau pe primul ajuns in coada de procese gata de executie
        if (proces_curent_executat.pid == -1 && !empty(&procese_ready)) {
            proces_curent_executat = front(&procese_ready);
            pop(&procese_ready);
            proces_curent_executat.start_time = time;
            proces_curent_executat.time_quantum = time_quantum;
            proces_curent_executat.wait_time += time - proces_curent_executat.last_active;
            if (proces_curent_executat.response_time == -1) {
                proces_curent_executat.response_time = time - proces_curent_executat.arrival;
            }
        }


        /// Daca am un proces, dar a expirat cuanta de timp il schimb
        if (proces_curent_executat.pid != -1 && proces_curent_executat.time_quantum == 0) {
            proces_curent_executat.last_active = time;
            push(&procese_ready, newNode(proces_curent_executat));
            if (!empty(&procese_ready)) {
                proces_curent_executat = front(&procese_ready);
                pop(&procese_ready);
                proces_curent_executat.wait_time += time - proces_curent_executat.last_active;
                proces_curent_executat.time_quantum = time_quantum;
                if (proces_curent_executat.response_time == -1) {
                    proces_curent_executat.response_time = time - proces_curent_executat.arrival;
                }
            }
        }


        /// printf("La timpul %d ruleaza procesul %d iar coada ready este: \n", time, proces_curent_executat.pid);
        ///  PrintQueue(&procese_ready);
        ///  printf("\n");
        //// Daca am un proces in executie atunci il execut pana se termina time_quantum-ul curent
        if (proces_curent_executat.pid != -1) {
            stats.cpu_utilization++;
            proces_curent_executat.remaining--;
            proces_curent_executat.time_quantum--;
            if (proces_curent_executat.remaining == 0) {
                stats.average_waiting_time += proces_curent_executat.wait_time;
                stats.average_turnaround_time += time - proces_curent_executat.arrival + 1;
                stats.average_response_time += proces_curent_executat.response_time;
                proces_curent_executat.pid = -1;
            }
        }
        time ++;
    }
    stats.average_turnaround_time /= procese.num_procese;
    stats.average_response_time /= procese.num_procese;
    stats.average_waiting_time /= procese.num_procese;
    stats.cpu_utilization = 100 * stats.cpu_utilization / time;
    return stats;
}

statistici PrioritySched(queue procese) {
    queue procese_ready; /// Coada cu procesele gata de exceturie
    init(&procese_ready);
    int time = 0; /// Timpul curent
    proces proces_curent_executat; /// Procesul care e in executie
    proces_curent_executat.pid = -1;
    statistici stats; /// Statiscile algoritmului
    while (1) {
        /// Cand nu mai vine niciun proces, nu execut niciun proces si nu mai am procese gata de executie inseamana ca s-a terminat
        if (proces_curent_executat.pid == -1 && empty(&procese_ready) && empty(&procese)) break;

        /// Adaug in coada cu procele gata de executie procesele ce au ajuns la timpul 'timp'
        if (!empty(&procese)) {
            proces aux;
            while (!empty(&procese)) {
                aux = front(&procese);
                if (aux.arrival == time) {
                    push(&procese_ready, newNode(aux));
                    pop(&procese);
                } else break;
            }
        }

        //// Daca in prezent nu execut niciun proces il iau pe primul ajuns in coada de procese gata de executie
        if (proces_curent_executat.pid == -1 && !empty(&procese_ready)) {
            proces_curent_executat = get_min_priority(&procese_ready);
            delete_pid(&procese_ready, proces_curent_executat.pid);
            if (proces_curent_executat.response_time == -1) {
                proces_curent_executat.response_time = time - proces_curent_executat.arrival;
            }
            proces_curent_executat.start_time = time;
        }

        //// Daca am un proces in executie atunci il execut
        if (proces_curent_executat.pid != -1) {
            stats.cpu_utilization++;
            proces_curent_executat.remaining--;
            if (proces_curent_executat.remaining == 0) {
                stats.average_waiting_time += proces_curent_executat.start_time - proces_curent_executat.arrival;
                stats.average_turnaround_time += time - proces_curent_executat.arrival + 1;
                stats.average_response_time += proces_curent_executat.response_time;
                proces_curent_executat.pid = -1;
            }
        }

        time++;
    }
    stats.average_turnaround_time /= procese.num_procese;
    stats.average_waiting_time /= procese.num_procese;
    stats.cpu_utilization = 100 * stats.cpu_utilization / time;
    stats.average_response_time /= procese.num_procese;
    return stats;
}

statistici RoundRobinPrioritySched(queue procese, int time_quantum) {
    queue procese_ready; /// Coada cu procesele gata de exceturie
    init(&procese_ready);
    int time = 0; /// Timpul curent
    proces proces_curent_executat; /// Procesul care e in executie
    proces_curent_executat.pid = -1;
    statistici stats; /// Statiscile algoritmului

    while (1) {
        if (proces_curent_executat.pid == -1 && empty(&procese_ready) && empty(&procese)) break;


        if (!empty(&procese)) {
            proces aux;
            while (!empty(&procese)) {
                aux = front(&procese);
                if (aux.arrival == time) {
                    push(&procese_ready, newNode(aux));
                    pop(&procese);
                } else break;
            }
        }
        //// Daca in prezent nu execut niciun proces il iau pe primul ajuns in coada de procese gata de executie
        if (proces_curent_executat.pid == -1 && !empty(&procese_ready)) {
            proces_curent_executat = front(&procese_ready);
            pop(&procese_ready);
            proces_curent_executat.start_time = time;
            proces_curent_executat.time_quantum = time_quantum;
            proces_curent_executat.wait_time += time - proces_curent_executat.last_active;
        }

        /// Daca am un proces, dar a expirat cuanta de timp il schimb
        if (proces_curent_executat.pid != -1 && proces_curent_executat.time_quantum == 0) {
            proces_curent_executat.last_active = time;
            push(&procese_ready, newNode(proces_curent_executat));
            if (!empty(&procese_ready)) {
                proces_curent_executat = get_min_priority(&procese_ready);
                delete_pid(&procese_ready, proces_curent_executat.pid);
                proces_curent_executat.wait_time += time - proces_curent_executat.last_active;
                proces_curent_executat.time_quantum = time_quantum;
            }
        }

        /// printf("La timpul %d ruleaza procesul %d iar coada ready este: \n", time, proces_curent_executat.pid);
        ///  PrintQueue(&procese_ready);
        ///  printf("\n");
        //// Daca am un proces in executie atunci il execut pana se termina time_quantum-ul curent
        if (proces_curent_executat.pid != -1) {
            stats.cpu_utilization++;
            proces_curent_executat.remaining--;
            proces_curent_executat.time_quantum--;
            if (proces_curent_executat.remaining == 0) {
                /// printf("Exit time: %d %d %d\n", time, proces_curent_executat.pid, proces_curent_executat.wait_time);
                stats.average_waiting_time += proces_curent_executat.wait_time;
                stats.average_turnaround_time += time - proces_curent_executat.arrival + 1;
                proces_curent_executat.pid = -1;
            }
        }
        time ++;
    }
    stats.average_turnaround_time /= procese.num_procese;
    stats.average_waiting_time /= procese.num_procese;
    stats.cpu_utilization = 100 * stats.cpu_utilization / time;
    return stats;
}

statistici MaximumPrioriyFirst(queue procese) {
    queue procese_ready; /// Coada cu procesele gata de exceturie
    init(&procese_ready);
    int time = 0; /// Timpul curent
    proces proces_curent_executat; /// Procesul care e in executie
    proces_curent_executat.pid = -1;
    statistici stats; /// Statiscile algoritmului
    while (1) {
        /// Cand nu mai vine niciun proces, nu execut niciun proces si nu mai am procese gata de executie inseamana ca s-a terminat
        if (proces_curent_executat.pid == -1 && empty(&procese_ready) && empty(&procese)) break;

        /// Adaug in coada cu procele gata de executie procesele ce au ajuns la timpul 'timp' si vad procesul cu cel mai mare cpu burst

        if (!empty(&procese)) {
            proces aux;
            while (!empty(&procese)) {
                aux = front(&procese);
                if (aux.arrival == time) {
                    push(&procese_ready, newNode(aux));
                    pop(&procese);
                } else break;
            }
        }

        /// La fiecare pas rulez procesul cu timpul de executie ramas cel mai mare
        if (proces_curent_executat.pid != -1) {
            proces_curent_executat.last_active = time;
            push(&procese_ready, newNode(proces_curent_executat));
        }

        if (!empty(&procese_ready)) {
            proces_curent_executat = get_min_priority(&procese_ready);
            delete_pid(&procese_ready, proces_curent_executat.pid);
            proces_curent_executat.wait_time += time - proces_curent_executat.last_active;

            printf("%d ", proces_curent_executat.pid);
            stats.cpu_utilization++;
            proces_curent_executat.remaining--;
            if (proces_curent_executat.remaining == 0) {
                stats.average_waiting_time += proces_curent_executat.wait_time;
                stats.average_turnaround_time += time - proces_curent_executat.arrival + 1;
                proces_curent_executat.pid = -1;
            }
        }


        time++;
    }
    stats.average_turnaround_time /= procese.num_procese;
    stats.average_waiting_time /= procese.num_procese;
    stats.cpu_utilization = 100 * stats.cpu_utilization / time;
    return stats;
}

statistici HighestRatioNext(queue procese) {
    queue procese_ready; /// Coada cu procesele gata de exceturie
    init(&procese_ready);
    int time = 0; /// Timpul curent
    proces proces_curent_executat; /// Procesul care e in executie
    proces_curent_executat.pid = -1;
    statistici stats; /// Statiscile algoritmului
    while (1) {
        /// Cand nu mai vine niciun proces, nu execut niciun proces si nu mai am procese gata de executie inseamana ca s-a terminat
        if (proces_curent_executat.pid == -1 && empty(&procese_ready) && empty(&procese)) break;

        /// Adaug in coada cu procele gata de executie procesele ce au ajuns la timpul 'timp'
        if (!empty(&procese)) {
            proces aux;
            while (!empty(&procese)) {
                aux = front(&procese);
                if (aux.arrival == time) {
                    push(&procese_ready, newNode(aux));
                    pop(&procese);
                } else break;
            }
        }
        //// Daca in prezent nu execut niciun proces il iau pe cel cu prioritate maxima
        if (proces_curent_executat.pid == -1 && !empty(&procese_ready)) {
            proces_curent_executat = get_max_response_ration(&procese_ready);
            delete_pid(&procese_ready, proces_curent_executat.pid);
            proces_curent_executat.start_time = time;
        }


        //// Daca am un proces in executie atunci il execut
        if (proces_curent_executat.pid != -1) {
            stats.cpu_utilization++;
            proces_curent_executat.remaining--;
            if (proces_curent_executat.remaining == 0) {
                stats.average_waiting_time += proces_curent_executat.start_time - proces_curent_executat.arrival;
                stats.average_turnaround_time += time - proces_curent_executat.arrival + 1;
                proces_curent_executat.pid = -1;
            }
        }

        time++;
    }
    stats.average_turnaround_time /= procese.num_procese;
    stats.average_waiting_time /= procese.num_procese;
    stats.average_turnaround_time /= procese.num_procese;
    stats.cpu_utilization = 100 * stats.cpu_utilization / time;
    return stats;
}

statistici HighestRatioNextPreemtive(queue procese) {
    queue procese_ready; /// Coada cu procesele gata de exceturie
    init(&procese_ready);
    int time = 0; /// Timpul curent
    proces proces_curent_executat; /// Procesul care e in executie
    proces_curent_executat.pid = -1;
    statistici stats; /// Statiscile algoritmului
    while (1) {
        /// Cand nu mai vine niciun proces, nu execut niciun proces si nu mai am procese gata de executie inseamana ca s-a terminat
        if (proces_curent_executat.pid == -1 && empty(&procese_ready) && empty(&procese)) break;

        /// Adaug in coada cu procele gata de executie procesele ce au ajuns la timpul 'timp' si vad procesul cu cel mai mare cpu burst

        if (!empty(&procese)) {
            proces aux;
            while (!empty(&procese)) {
                aux = front(&procese);
                if (aux.arrival == time) {
                    push(&procese_ready, newNode(aux));
                    pop(&procese);
                } else break;
            }
        }

        /// La fiecare pas rulez procesul cu timpul de executie ramas cel mai mare
        if (proces_curent_executat.pid != -1) {
            proces_curent_executat.last_active = time;
            push(&procese_ready, newNode(proces_curent_executat));
        }

        if (!empty(&procese_ready)) {
            proces_curent_executat = get_max_response_ration(&procese_ready);
            delete_pid(&procese_ready, proces_curent_executat.pid);
            proces_curent_executat.wait_time += time - proces_curent_executat.last_active;

            stats.cpu_utilization++;
            proces_curent_executat.remaining--;
            if (proces_curent_executat.remaining == 0) {
                stats.average_waiting_time += proces_curent_executat.wait_time;
                stats.average_turnaround_time += time - proces_curent_executat.arrival + 1;
                proces_curent_executat.pid = -1;
            }
        }


        time++;
    }
    stats.average_turnaround_time /= procese.num_procese;
    stats.average_waiting_time /= procese.num_procese;
    stats.cpu_utilization = 100 * stats.cpu_utilization / time;
    stats.average_turnaround_time /= procese.num_procese;
    return stats;
}

void AfisareStatistici(statistici stats) {
    printf("CPU Utilization: %f\n Average turnaround time: %f\n Average waiting time: %f\n Average response time: %f\n", stats.cpu_utilization, stats.average_turnaround_time, stats.average_waiting_time, stats.average_response_time);
}

int main(int argc, char* argv[]) {
    FILE* fisier = fopen(argv[1], "r");
    char alg[5];
    strcpy(alg, argv[2]);
    int time_quant;
    queue procese;
    proces aux;
    int cnt = 1;
    init(&procese);
    while (fscanf(fisier, "%d%d%d%d", &aux.pid,&aux.arrival, &aux.remaining, &aux.priority) == 4) {
        cnt++;
        aux.start_time = -1;
        aux.wait_time = 0;
        aux.last_active = aux.arrival;
        aux.response_time = -1;
        push(&procese, newNode(aux));
    }
    procese.num_procese = cnt-1;
    fclose(fisier);
    statistici stats;
    if (strcmp(alg, "FCFS") == 0) {
        stats = FirstComeFirstServe(procese);
    }
    if (strcmp(alg, "SJF") == 0) {
        stats = ShortestJobFirst(procese);
    }
    if (strcmp(alg, "SRTM") == 0) {
        stats = ShortestRemainingTimeFirst(procese);
    }
    if (strcmp(alg, "LJF") == 0) {
        stats = LongestJobFirst(procese);
    }
    if (strcmp(alg, "LRJF") == 0) {
        stats = LongestRemaningTimeFirst(procese);
    }
    if (strcmp(alg, "RR") == 0) {
        time_quant = atoi(argv[3]);
        stats = RoundRobin(procese, time_quant);
    }
    if (strcmp(alg, "PR") == 0) {
        stats = PrioritySched(procese);
    }
    if (strcmp(alg, "RRP") == 0) {
        time_quant = atoi(argv[3]);
        stats = RoundRobinPrioritySched(procese, time_quant);
    }
    if (strcmp(alg, "MPF") == 0) {
        stats = MaximumPrioriyFirst(procese);
    }
    if (strcmp(alg, "HRN") == 0) {
        stats = HighestRatioNext(procese);
    }
    if (strcmp(alg, "HRNP") == 0) {
        stats = HighestRatioNextPreemtive(procese);
    }
    printf("%s\n", alg);
    AfisareStatistici(stats);

    return 0;
}

#include <iostream>
#include <algorithm>
#include <queue>
#include <limits>
using namespace std;
int kol_open_file = 0, kol_swap = 0;
struct MinHeapNode
{
    // элемент для сохранения
    int element;

    // индекс массива, из которого берется элемент
    int i;
};

// Объект сравнения, который будет использоваться для упорядочивания кучи
struct comp
{
    bool operator()(const MinHeapNode& lhs, const MinHeapNode& rhs) const {
        return lhs.element > rhs.element;
    }
};

FILE* openFile(char* fileName, const char* mode)
{
    FILE* fp = fopen(fileName, mode);
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }kol_open_file++;
    return fp;
}

// Объединяет 3 отсортированных файлов.
void mergeFiles(char* output_file, int n, int k)
{
    FILE* in[3];
    for (int i = 0; i < k; i++)
    {
        char fileName[2];

        // преобразовать `i` в строку
        snprintf(fileName, sizeof(fileName), "%d", i);

        // открываем выходные файлы в режиме чтения
        in[i] = openFile(fileName, "r");
    }

    // КОНЕЧНЫЙ ВЫВОДНОЙ ФАЙЛ
    FILE* out = openFile(output_file, "w");

    // Создаем мини-кучу с 3 узлами кучи
    MinHeapNode harr[3];
    priority_queue<MinHeapNode, vector<MinHeapNode>, comp> pq;

    int i;
    for (i = 0; i < k; i++)
    {
        // прерываем, если нет пустого выходного файла и
        // индекс `i` будет количеством входных файлов
        if (fscanf(in[i], "%d ", &harr[i].element) != 1) {
            break;
        }

        // индекс рабочего файла вывода
        harr[i].i = i;
        pq.push(harr[i]);
    }

    int count = 0;

    // Один за другим получаем минимальный элемент из минимальной кучи и заменяем
    // это со следующим элементом. Выполнять до тех пор, пока все заполненные входные файлы не достигнут EOF.
    while (count != i)
    {
        // Получить минимальный элемент и сохранить его в выходном файле
        MinHeapNode root = pq.top();
        pq.pop();
        fprintf(out, "%d ", root.element);

        // Находим следующий элемент, который должен заменить текущий корень кучи.
        // Следующий элемент принадлежит тому же входному файлу, что и текущий
        // минимальный элемент.
        if (fscanf(in[root.i], "%d ", &root.element) != 1)
        {
            root.element = numeric_limits<int>::max();
            count++; kol_swap++;
        }

        // Заменяем корень следующим элементом входного файла
        pq.push(root);
    }

    // закрываем входной и выходной файлы
    for (int i = 0; i < k; i++) {
        fclose(in[i]);
    }

    fclose(out);
}

// Используя алгоритм сортировки слиянием, создайте начальные прогоны и разделите их
// равномерно среди выходных файлов
void createInitialRuns(char* input_file, int run_size, const int num_ways)
{
    // Для большого входного файла
    FILE* in = openFile(input_file, "r");

    // вывод рабочих файлов
    FILE* out[3];
    char fileName[2];
    for (int i = 0; i < num_ways; i++)
    {
        // преобразовать `i` в строку
        snprintf(fileName, sizeof(fileName), "%d", i);

        // Открыть выходные файлы в режиме записи.
        out[i] = openFile(fileName, "w");
    }

    // выделяем динамический массив, достаточно большой для размещения прогонов
    // размер `run_size`
    int* arr = new int[run_size];

    bool more_input = true;
    int next_output_file = 0;

    int i;
    while (more_input)
    {
        // записать элементы `run_size` в `arr` из входного файла
        for (i = 0; i < run_size; i++)
        {
            if (fscanf(in, "%d ", &arr[i]) != 1)
            {
                more_input = false;
                break;
            }
        }

        // сортируем массив
        sort(arr, arr + i); kol_swap++;

        // записываем записи в соответствующий временной выходной файл
        // нельзя предположить, что цикл работает до `run_size`
        // так как длина последнего прогона может быть меньше `run_size`
        for (int j = 0; j < i; j++) {
            fprintf(out[next_output_file], "%d ", arr[j]);
        }

        next_output_file++;
    }

    // освобождаем память
    delete arr;

    // закрываем входной и выходной файлы
    for (int i = 0; i < num_ways; i++) {
        fclose(out[i]);
    }

    fclose(in);
}

int main()
{
    // количество разделов входного файла
    int num_ways = 3;

    // размер каждого раздела
    int run_size = 500;

    char input_file[] = "input.txt";
    char output_file[] = "output.txt";

    FILE* in = openFile(input_file, "w");

    srand(time(NULL));

    // генерируем ввод
    for (int i = 0; i < num_ways * run_size; i++) {
        fprintf(in, "%d ", rand());
    }

    fclose(in);

    // Читаем входной файл, создаем начальные прогоны,
    // и назначаем прогоны рабочим файлам вывода
    createInitialRuns(input_file, run_size, num_ways);
    time_t start, end; time(&start);
    // Объединяем прогоны, используя слияние по k-путям
    mergeFiles(output_file, run_size, num_ways);
    time(&end); double seconds = difftime(end, start);
    printf("The time: %f ms\n", seconds);
    return 0;
}
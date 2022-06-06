#include <iostream>
#include <memory>
#include <vector>
#include <limits>

static void calculate_weight(int begin_index, const std::vector<std::vector<int>> &tree, std::vector<int> &weights) {
    int min_index;
    int min;
    bool visited[tree.size()];

    weights = std::vector<int>(tree.size());
    for (auto &m: weights) {
        m = std::numeric_limits<int>::max();
    }
    for (auto &v: visited) {
        v = true;
    }

    weights[begin_index] = 0;
    do {
        min_index = std::numeric_limits<int>::max();
        min = std::numeric_limits<int>::max();
        for (int i = 0; i < tree.size(); ++i) {
            if (visited[i] && weights[i] < min) {
                min = weights[i];
                min_index = i;
            }
        }
        if (min_index != std::numeric_limits<int>::max()) {
            for (int i = 0; i < tree.size(); ++i) {
                if (tree[min_index][i] > 0) {
                    int n = min + tree[min_index][i];
                    if (n < weights[i]) {
                        weights[i] = n;
                    }
                }
            }
            visited[min_index] = false;
        }
    } while (min_index < std::numeric_limits<int>::max());
    std::cout << "Min : [";
    for (int i = 0; i < tree.size(); ++i) {
        if (i > 0)
            std::cout << ", ";
        std::cout << weights[i];
    }
    std::cout << "]" << std::endl;
}

static void calculate_path(int begin_index, int end, const std::vector<std::vector<int>> &tree,
                           const std::vector<int> &weights, std::vector<int> &path) {
    int weight = weights[end];

    path.assign(0, 0);
    path.insert(path.begin(), end);
    while (end != begin_index) {
        for (int i = 0; i < tree.size(); ++i) {
            if (tree[i][end] != 0) {
                int t = weight - tree[i][end];
                if (t == weights[i]) {
                    weight = t;
                    end = i;
                    path.insert(path.begin(), i);
                }
            }
        }
    }

    std::cout << "Path: [";
    for (int i = 0; i < path.size(); ++i) {
        if (i > 0)
            std::cout << ", ";
        std::cout << path[i];
    }
    std::cout << "]" << std::endl;
}

int
main(int argc, char **argv) {
    std::vector<int> weights, path;
    std::vector<std::vector<int>> tree = {
            {0,  7,  9,  0,  0, 14},
            {7,  0,  10, 15, 0, 0},
            {9,  10, 0,  11, 0, 2},
            {0,  15, 11, 0,  6, 0},
            {0,  0,  0,  6,  0, 9},
            {14, 0,  2,  0,  9, 0}
    };
    calculate_weight(0, tree, weights);
    calculate_path(0, 4, tree, weights, path);
    return EXIT_SUCCESS;
}

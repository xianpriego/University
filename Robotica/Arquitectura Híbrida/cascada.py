from warnings import warn
import heapq

def water(map, start, end):
    working_matrix = map
    visited = set()
    wavefront = [(end[0], end[1])]

    distance = 0

    while wavefront:
        next_wave = []

        for x, y in wavefront:
            temp= working_matrix[x][y]
            if working_matrix[x][y] == 1:
                working_matrix[x][y] = 50 
            else:
                working_matrix[x][y] = distance

            visited.add((x, y))

            neighbors = [(x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1),
                        (x - 1, y - 1), (x - 1, y + 1), (x + 1, y - 1), (x + 1, y + 1)]

            for nx, ny in neighbors:
                if 0 <= nx < len(working_matrix) and 0 <= ny < len(working_matrix[0]) and (nx, ny) not in visited:
                    next_wave.append((nx, ny))
                    visited.add((nx, ny))

        wavefront = next_wave
        distance += 1

    current = start
    path = [current]
    frontera = []
    visitados = [current]
    heapq.heappush(frontera, (working_matrix[start[0]][start[1]], start))
    while current != end:
        neighbors = [(current[0] - 1, current[1]), (current[0] + 1, current[1]), (current[0], current[1] - 1), (current[0], current[1] + 1)]
        for n in neighbors:
            if 0 <= n[0] < len(working_matrix) and 0 <= n[1] < len(working_matrix[0]) and n not in visitados:
                distance = working_matrix[n[0]][n[1]]
                heapq.heappush(frontera, (distance, n))
        _, next = heapq.heappop(frontera)
        path.append(next)
        visitados.append(next)
        current = next

    i = 0
    while i < len(path) - 1:
        x1, y1 = path[i]
        x2, y2 = path[i + 1]
        if abs(x2 - x1) > 1 or abs(y2 - y1) > 1:
            del path[i]
        else:
            i += 1
    return path
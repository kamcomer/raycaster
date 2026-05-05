import pygame
from screen_area import ScreenArea
from event_bus import EventBus


class MapEditor:
    def __init__(
        self,
        map_size: tuple[int, int],
        area: ScreenArea,
        event_bus: EventBus | None = None,
    ) -> None:
        self._area = area

        self.map_size: tuple[int, int] = map_size
        self.grid_cells: list[pygame.Rect]
        self.grid_cell_size: tuple[int, int]
        self.grid_boarder_size: tuple[int, int]

        # self.walls = [map_size[0]][map_size[1]]
        # self.floors = [map_size[0]][map_size[1]]
        # self.ceil = [map_size[0]][map_size[1]]

        self.textures = []
        self.sprites = []

        if event_bus is not None:
            self.subscribe_to_events(event_bus)

        self.create_grid()
        self.draw_grid_lines()

    def subscribe_to_events(self, event_bus: EventBus):
        event_bus.subscribe(pygame.MOUSEBUTTONDOWN, self.handle_click)
        event_bus.subscribe(pygame.WINDOWRESIZED, self.handle_resize)

    def update(self):
        pass

    def _calc_grid_boarder(self):
        return (
            self.area_size[0] % self.map_size[0] // 2,
            self.area_size[1] % self.map_size[1] // 2,
        )

    def _calc_grid_cell_size(self):
        return (
            (self.area_size[0] - self.grid_boarder_size[0]) // self.map_size[0],
            (self.area_size[1] - self.grid_boarder_size[1]) // self.map_size[1],
        )

    @property
    def area(self):
        if not self._area:
            raise ValueError("Area not set")

        return self._area

    @area.setter
    def area(self, value: ScreenArea):
        self._area = value

    @property
    def area_size(self):
        return self._area.size

    def handle_click(self, event):
        if not self.area.is_in_area(event.pos):
            return

        editor_pos = self.area.screen_to_area_pos(event.pos)

        col = (editor_pos[0] - self.grid_boarder_size[0]) // self.grid_cell_size[0]
        row = (editor_pos[1] - self.grid_boarder_size[1]) // self.grid_cell_size[1]

        if col < 0:
            col = 0

        if row < 0:
            row = 0

        idx = int(row * self.map_size[0] + col)
        target_cell = self.grid_cells[idx]
        if not target_cell.collidepoint(editor_pos):
            return

        self.area.surface.fill("#ff0000", target_cell)

    def handle_resize(self, event):
        self.create_grid()
        self.draw_grid_lines()

    # def update(self):
    #     self.area.blit()

    def create_grid(self):
        self.grid_boarder_size = self._calc_grid_boarder()
        self.grid_cell_size = self._calc_grid_cell_size()
        self.grid_cells = []

        for y in range(self.map_size[1]):
            for x in range(self.map_size[0]):
                rect = pygame.Rect(
                    (
                        self.grid_cell_size[0] * x + self.grid_boarder_size[0] + 1,
                        self.grid_cell_size[1] * y + self.grid_boarder_size[1] + 1,
                    ),
                    (self.grid_cell_size[0] - 1, self.grid_cell_size[1] - 1),
                )
                self.grid_cells.append(rect)

    def draw_grid_lines(
        self,
        color: pygame.Color = pygame.Color(255, 255, 255),
    ):
        y_offset = self.grid_boarder_size[1]
        x_offset = self.grid_boarder_size[0]

        for _ in range(self.map_size[1] + 1):
            pygame.draw.line(
                self.area.surface,
                color,
                (self.grid_boarder_size[0], y_offset),
                (self.area.size[0] - self.grid_boarder_size[0] - 1, y_offset),
            )

            y_offset += self.grid_cell_size[1]

        for _ in range(self.map_size[0] + 1):
            pygame.draw.line(
                self.area.surface,
                color,
                (x_offset, self.grid_boarder_size[1]),
                (x_offset, self.area.size[1] - self.grid_boarder_size[1] - 1),
            )

            x_offset += self.grid_cell_size[0]

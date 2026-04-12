import pygame
import pygame_gui
import sys
import PIL

DEFAULT_SCREEN_SIZE = (800, 450)
DEFAULT_MAP_AREA_SIZE = (650, 450)
DEFAULT_PALLET_AREA_SIZE = (150, 450)

DEFAULT_MAP_GRID_SIZE = (24, 24)
TICK_RATE = 60


class UIArea:
    def __init__(
        self,
        target_surface: pygame.Surface,
        size_ratio: tuple[float, float],
        manager: pygame_gui.UIManager,
        offset_ratio: tuple[float, float] = (0.0, 0.0),
        bg_color: pygame.Color = pygame.Color("#000000"),
    ) -> None:
        self.target_surface = target_surface
        self.size_ratio = size_ratio
        self.offset_ratio = offset_ratio

        self.surface = pygame.Surface(
            (
                target_surface.size[0] * size_ratio[0],
                target_surface.size[1] * size_ratio[1],
            )
        )
        self.bg_color = bg_color

        self.ui_container = pygame_gui.elements.UIAutoResizingContainer(
            self.surface.get_rect(), manager=manager
        )

    @property
    def size(self) -> pygame.Vector2:
        size = self.surface.size
        return pygame.Vector2(size[0], size[1])

    def is_in_area(self, point: tuple[int, int]):
        return self.surface.get_rect().collidepoint(point)

    def blit(self):
        offset = (
            self.target_surface.size[0] * self.offset_ratio[0],
            self.target_surface.size[1] * self.offset_ratio[1],
        )
        self.target_surface.blit(self.surface, offset)

    def add_ui_component(self, component: pygame_gui.core.UIElement):
        self.ui_container.add_element(component)

    def remove_ui_component(self, component: pygame_gui.core.UIElement):
        self.ui_container.remove_element(component)

    def resize(self, screen_size: tuple[int, int]):
        surface_size = (
            screen_size[0] * self.size_ratio[0],
            screen_size[1] * self.size_ratio[1],
        )
        self.surface = pygame.Surface(surface_size)
        self.ui_container.relative_rect = self.surface.get_rect()


class Map:
    def __init__(self, map_size: tuple[int, int], area: UIArea | None = None) -> None:
        self._area = area

        self.map_size = pygame.Vector2(map_size[0], map_size[1])
        self.grid_cells: list[pygame.Rect]
        self.grid_cell_size: pygame.Vector2
        self.grid_cell_size: pygame.Vector2

        # self.walls = [map_size[0]][map_size[1]]
        # self.floors = [map_size[0]][map_size[1]]
        # self.ceil = [map_size[0]][map_size[1]]

        self.textures = []
        self.sprites = []

        self.create_grid()
        self.draw_grid()

    @property
    def area(self):
        if not self._area:
            raise ValueError("Area not set")

        return self._area

    @area.setter
    def surface(self, value: UIArea):
        self._area = value

    def handle_click(self, event):
        if not self.area.is_in_area(event.pos):
            return

        col = (event.pos[0] - self.grid_boarder_size.x) // self.grid_cell_size.x
        row = (event.pos[1] - self.grid_boarder_size.y) // self.grid_cell_size.y

        if col < 0:
            col = 0

        if row < 0:
            row = 0

        idx = int(row * self.map_size.x + col)
        target_cell = self.grid_cells[idx]
        if not target_cell.collidepoint(event.pos):
            return

        self.area.surface.fill("#ff0000", target_cell)

    def handle_resize(self, event):
        self.area.resize((event.x, event.y))
        self.create_grid()
        self.draw_grid()

    def resize_map(self, size: tuple[int, int]):
        pass

    def update_tile(self, pos: tuple[int, int]):
        pass

    def update(self):
        self.area.blit()

    def create_grid(self):
        self.grid_boarder_size = pygame.Vector2(
            self.surface.size.x % self.map_size.x // 2,
            self.surface.size.y % self.map_size.y // 2,
        )
        self.grid_cell_size = pygame.Vector2(
            (self.surface.size.x - self.grid_boarder_size.x) // self.map_size.x,
            (self.surface.size.y - self.grid_boarder_size.y) // self.map_size.y,
        )

        self.grid_cells = []

        for y in range(int(self.map_size.y)):
            for x in range(int(self.map_size.x)):
                rect = pygame.Rect(
                    (
                        self.grid_cell_size.x * x + self.grid_boarder_size.x,
                        self.grid_cell_size.y * y + self.grid_boarder_size.y,
                    ),
                    (self.grid_cell_size.x, self.grid_cell_size.y),
                )
                self.grid_cells.append(rect)

    def draw_grid(
        self,
        color: pygame.Color = pygame.Color(255, 255, 255),
    ):
        y_offset = self.grid_boarder_size.y + self.grid_cell_size.y
        x_offset = self.grid_boarder_size.x + self.grid_cell_size.x

        for _ in range(int(self.map_size.y - 1)):
            pygame.draw.line(
                self.area.surface,
                color,
                (self.grid_boarder_size.x, y_offset),
                (self.area.size.x - self.grid_boarder_size.x - 1, y_offset),
            )

            y_offset += self.grid_cell_size.y

        for _ in range(int(self.map_size.x - 1)):
            pygame.draw.line(
                self.area.surface,
                color,
                (x_offset, self.grid_boarder_size.y),
                (x_offset, self.area.size.y - self.grid_boarder_size.y - 1),
            )

            x_offset += self.grid_cell_size.x


class TexturePallet:
    def __init__(self, size: tuple[int, int], surface: pygame.Surface) -> None:
        self._area = None
        self.textures = []
        self.sprites = []

    @property
    def area(self):
        if not self._area:
            raise ValueError("Area not set")

        return self._area

    @area.setter
    def surface(self, value: UIArea):
        self._area = value


event_handlers = {
    pygame.QUIT: [],
    pygame.WINDOWRESIZED: [],
    pygame.MOUSEBUTTONDOWN: [],
}


def main():
    screen_size = DEFAULT_SCREEN_SIZE

    pygame.init()
    screen = pygame.display.set_mode(screen_size, pygame.RESIZABLE)
    clock = pygame.time.Clock()

    # pallet_rect = pygame.Rect((0, 0), DEFAULT_PALLET_SIZE)
    # pallet_surface = pygame.Surface(DEFAULT_PALLET_SIZE)
    # pallet_surface.fill(pygame.Color("#000000"))

    manager = pygame_gui.UIManager(screen_size, theme_path="theme.json")

    map_area = UIArea(screen, (0.8, 1), manager)
    map = Map(DEFAULT_MAP_GRID_SIZE, map_area)

    # texture_pallet_area = UIArea(
    #     screen, (0.8, 1), manager, (0.75, 0), surface_color=pygame.Color("#ff0000")
    # )
    # texture_pallet = Map(DEFAULT_PALLET_AREA_SIZE, texture_pallet_area)

    # texture_pallet_area = UIArea(
    #     screen, 0.25, DEFAULT_PALLET_AREA_SIZE, manager, map_area.surface.size
    # )
    # texture_pallet = TexturePallet()

    event_handlers[pygame.MOUSEBUTTONDOWN].append(map.handle_click)
    event_handlers[pygame.WINDOWRESIZED].append(map.handle_resize)

    # screen_container = pygame_gui.elements.UIAutoResizingContainer(
    #     screen_rect, manager=manager
    # )
    #
    # pallet_rect = pygame.Rect((0, 0), (150, 600))
    # pallet_rect.topright = (0, 0)
    # pallet_container = pygame_gui.elements.UIAutoResizingContainer(
    #     pallet_rect, manager=manager, anchors={"right": "right", "top": "top"}
    # )
    #
    # hello_button = pygame_gui.elements.UIButton(
    #     relative_rect=pygame.Rect((0, 0), (100, 50)),
    #     text="Say Hello",
    #     manager=manager,
    #     container=pallet_container,
    # )
    #
    # hello_button2 = pygame_gui.elements.UIButton(
    #     relative_rect=pygame.Rect((0, 500), (100, 50)),
    #     text="Say Hello",
    #     manager=manager,
    #     container=pallet_container,
    # )
    #
    # hello_button3 = pygame_gui.elements.UIButton(
    #     relative_rect=pygame.Rect((0, 600), (100, 50)),
    #     text="Say Hello",
    #     manager=manager,
    #     parent_element=pallet_container,
    # )

    while True:
        dt = clock.tick(TICK_RATE) / 1000.0
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

            if event.type in event_handlers:
                pygame.Event
                for callback in event_handlers[event.type]:
                    callback(event)

            # if event.type == pygame_gui.UI_BUTTON_PRESSED:
            #     if event.ui_element == hello_button:
            #         print("Hello World!")
            manager.process_events(event)

        screen.fill("#000000")
        manager.update(dt)
        map.update()
        manager.draw_ui(screen)
        pygame.display.update()


if __name__ == "__main__":
    main()

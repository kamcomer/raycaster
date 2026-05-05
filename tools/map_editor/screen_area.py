import pygame
from dataclasses import dataclass


@dataclass
class ScreenAreaBounds:
    x_min: int = -1
    y_min: int = -1
    x_max: int = -1
    y_max: int = -1


class ScreenArea:
    def __init__(
        self,
        surface: pygame.Surface,
        pos: tuple[int, int],
        size_ratio: tuple[float, float],
        offset_ratio: tuple[float, float] = (0.0, 0.0),
        bounds: ScreenAreaBounds = ScreenAreaBounds(),
        bg_color: pygame.Color = pygame.Color("#000000"),
    ) -> None:
        self.size_ratio = size_ratio
        self.offset_ratio = offset_ratio
        self.bg_color = bg_color
        self.bounds = bounds

        self.pos = pos
        self.surface = surface

    @property
    def size(self) -> tuple[int, int]:
        return self.surface.get_size()

    @property
    def rect(self):
        surface_size = self.surface.get_size()
        return pygame.Rect(0, 0, *surface_size)

    @property
    def abs_rect(self):
        surface_size = self.surface.get_size()
        return pygame.Rect(
            self.pos[0], self.pos[1], *surface_size
        )

    def screen_to_area_pos(self, screen_pos: tuple[int, int]):
        return (screen_pos[0] - self.pos[0], screen_pos[1] - self.pos[1])

    def is_in_area(self, point: tuple[int, int]):
        return self.abs_rect.collidepoint(point)

    def blit(self, surface: pygame.Surface):
        surface.blit(self.surface, self.pos)

    def resize(self, pos, new_surface):
        self.pos = pos
        self.surface = new_surface
        self.clear()

    def clear(self):
        if self.surface is None:
            raise (ValueError)
        self.surface.fill(self.bg_color)

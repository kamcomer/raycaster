import pygame
from screen_area import ScreenArea, ScreenAreaBounds


class LayoutManager:
    def __init__(
        self,
        target_surface: pygame.Surface,
    ) -> None:
        self.target_surface = target_surface
        self.areas: list[ScreenArea] = []

    def add_area(self, area: ScreenArea):
        self.areas.append(area)

    def create_area(
        self,
        size_ratio: tuple[float, float],
        offset_ratio: tuple[float, float] = (0, 0),
        area_bounds: ScreenAreaBounds = ScreenAreaBounds(),
        bg_color: pygame.Color = pygame.Color("#000000"),
    ) -> ScreenArea:
        pos = self._calc_pos(offset_ratio)
        surface = pygame.Surface(
            self._calc_size(size_ratio, area_bounds), pygame.SRCALPHA
        )
        sa = ScreenArea(surface, pos, size_ratio, offset_ratio, area_bounds, bg_color)
        self.add_area(sa)
        return sa

    def _calc_size(self, size_ratio: tuple[float, float], bounds: ScreenAreaBounds):
        ratio_size = (
            int(self.target_surface.size[0] * size_ratio[0]),
            int(self.target_surface.size[1] * size_ratio[1]),
        )

        x = y = 0
        if bounds.x_min >= 0 and ratio_size[0] < bounds.x_min:
            x = bounds.x_min
        elif bounds.x_max > 0 and ratio_size[0] > bounds.x_max:
            x = bounds.x_max
        else:
            x = ratio_size[0]

        if bounds.y_min >= 0 and ratio_size[1] < bounds.y_min:
            y = bounds.y_min
        elif bounds.y_max > 0 and ratio_size[1] > bounds.y_max:
            y = bounds.y_max
        else:
            y = ratio_size[1]

        return (x, y)

    def _calc_pos(self, offset_ratio: tuple[float, float]):
        return (
            int(self.target_surface.size[0] * offset_ratio[0]),
            int(self.target_surface.size[1] * offset_ratio[1]),
        )

    def handle_resize(self, event):
        for area in self.areas:
            pos = self._calc_pos(area.offset_ratio)
            surface = pygame.Surface(
                self._calc_size(area.size_ratio, area.bounds), pygame.SRCALPHA
            )
            area.resize(pos, surface)

    def blit(self):
        for area in self.areas:
            self.target_surface.fill(area.bg_color, area.abs_rect)
            area.blit(self.target_surface)

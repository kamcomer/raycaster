import pygame
import pygame_gui


class EventBus:
    def __init__(self):
        self._handlers = {}
        self._element_handlers = {}

    def subscribe(self, event_type: int, callback):
        if event_type not in self._handlers:
            self._handlers[event_type] = []
        self._handlers[event_type].append(callback)

    def subscribe_element(self, event_type: int, element, callback):
        if event_type not in self._element_handlers:
            self._element_handlers[event_type] = {}
        self._element_handlers[event_type][element] = callback

    def unsubscribe(self, event_type: int, callback):
        if event_type in self._handlers:
            self._handlers[event_type].remove(callback)

    def dispatch(self, event):
        if event.type in self._element_handlers and hasattr(event, "ui_element"):
            callback = self._element_handlers[event.type].get(event.ui_element)
            if callback:
                callback()
                return

        if event.type in self._handlers:
            for callback in self._handlers[event.type]:
                callback(event)

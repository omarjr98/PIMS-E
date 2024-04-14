#ifndef GPIO_H
#define GPIO_H

void export_gpio(const char *gpio);
void set_direction(const char *gpio, const char *direction);
char read_value(const char *gpio);
void set_value(const char *gpio, const char *value);

#endif

/*
 * dpi AMOLED LCD drm_panel driver.
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd
 * Derived from drivers/video/backlight/dpi.c
 *
 * Andrzej Hajda <a.hajda@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <drm/drmP.h>
#include <drm/drm_panel.h>

#include <linux/gpio/consumer.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>
#include <video/of_videomode.h>
#include <video/videomode.h>

struct dpi {
	struct device *dev;
	struct drm_panel panel;

	struct videomode vm;
	u32 width_mm;
	u32 height_mm;
};

static inline struct dpi *panel_to_dpi(struct drm_panel *panel)
{
	return container_of(panel, struct dpi, panel);
}

static int dpi_disable(struct drm_panel *panel)
{
	return 0;
}

static int dpi_unprepare(struct drm_panel *panel)
{
	return 0;
}

static int dpi_prepare(struct drm_panel *panel)
{
	return 0;
}

static int dpi_enable(struct drm_panel *panel)
{
	return 0;
}

static int dpi_get_modes(struct drm_panel *panel)
{
	struct drm_connector *connector = panel->connector;
	struct dpi *ctx = panel_to_dpi(panel);
	struct drm_display_mode *mode;

	mode = drm_mode_create(connector->dev);
	if (!mode) {
		DRM_ERROR("failed to create a new display mode\n");
		return 0;
	}

	drm_display_mode_from_videomode(&ctx->vm, mode);
	mode->width_mm = ctx->width_mm;
	mode->height_mm = ctx->height_mm;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static const struct drm_panel_funcs dpi_drm_funcs = {
	.disable = dpi_disable,
	.unprepare = dpi_unprepare,
	.prepare = dpi_prepare,
	.enable = dpi_enable,
	.get_modes = dpi_get_modes,
};

static int dpi_parse_dt(struct dpi *ctx)
{
	struct device *dev = ctx->dev;
	struct device_node *np = dev->of_node;
	int ret;

	ret = of_get_videomode(np, &ctx->vm, 0);
	if (ret < 0)
		return ret;

	of_property_read_u32(np, "panel-width-mm", &ctx->width_mm);
	of_property_read_u32(np, "panel-height-mm", &ctx->height_mm);

	return 0;
}

static int dpi_probe(struct platform_device *pdev)
{
	struct dpi *ctx;
	int ret;

	ctx = devm_kzalloc(&pdev->dev, sizeof(struct dpi), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	platform_set_drvdata(pdev, ctx);

	ctx->dev = &pdev->dev;

	ret = dpi_parse_dt(ctx);
	if (ret < 0)
		return ret;

	drm_panel_init(&ctx->panel);
	ctx->panel.dev = &pdev->dev;
	ctx->panel.funcs = &dpi_drm_funcs;

	return drm_panel_add(&ctx->panel);
}

static int dpi_remove(struct platform_device *pdev)
{
	struct dpi *ctx = platform_get_drvdata(pdev);

	drm_panel_remove(&ctx->panel);

	return 0;
}

static const struct of_device_id dpi_of_match[] = {
	{ .compatible = "samsung,dpi" },
	{ }
};
MODULE_DEVICE_TABLE(of, platform_of_match);

static struct platform_driver dpi_driver = {
	.probe = dpi_probe,
	.remove = dpi_remove,
	.driver = {
		.name = "panel-samsung-dpi",
		.of_match_table = dpi_of_match,
	},
};
module_platform_driver(dpi_driver);

MODULE_AUTHOR("Andrzej Hajda <a.hajda@samsung.com>");
MODULE_DESCRIPTION("dpi LCD Driver");
MODULE_LICENSE("GPL v2");

#include "Saga/Interface/SagaUiPanelWidget.h"

USagaUiPanelWidget::USagaUiPanelWidget(const FObjectInitializer& initializer)
noexcept
	: Super(initializer)
{
	myTickPolicy = ESagaLiveUserWidgetTickPolicy::TickBySelf;
}

void
USagaUiPanelWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void
USagaUiPanelWidget::NativeTick(const FGeometry& geometry, float delta_time)
{
	Super::NativeTick(geometry, delta_time);

	auto& old_transform = GetRenderTransform();
	const FVector2D old_scale = old_transform.Scale;

	const FVector2D new_scale = CalculateTargetScale();
	const FVector2D scale_gap = new_scale - old_scale;

	const double xscale_delta = 3.0 * delta_time;
	const double yscale_delta = 2.0 * delta_time;

	double xscale, yscale;
	if (abs(scale_gap.X) < xscale_delta)
	{
		xscale = new_scale.X;
	}
	else
	{
		xscale = old_scale.X + FMath::Sign(scale_gap.X) * xscale_delta;
	}

	if (abs(scale_gap.Y) < yscale_delta)
	{
		yscale = new_scale.Y;
	}
	else
	{
		yscale = old_scale.Y + FMath::Sign(scale_gap.Y) * yscale_delta;
	}

	const FVector2D translation{};
	const FVector2D scale{ xscale, yscale };
	const FVector2D shear{};

	FWidgetTransform transform{ translation, new_scale, shear, 0 };
	//SetRenderTransform(transform);
}

FVector2D
USagaUiPanelWidget::CalculateTargetScale()
const noexcept
{
	const double prog = static_cast<double>(GetProgress());

	double target_xscale;
	double target_yscale;

	switch (myState)
	{
	case ESagaLiveUserWidgetStates::Opening:
	{
		// 0.0 to 1.0

		target_xscale = prog;
		target_yscale = prog;

		//target_xscale = FSagaEasingLibrary::CatmullRomSpline(prog, 0.0, 0.2, 0.9, 1.0);
		//target_yscale = FSagaEasingLibrary::CatmullRomSpline(prog, 0.0, 0.7, 1.2, 1.0);
	}
	break;

	case ESagaLiveUserWidgetStates::Idle:
	{
		target_xscale = 1.0f;
		target_yscale = 1.0f;
	}
	break;

	case ESagaLiveUserWidgetStates::Closing:
	{
		target_xscale = FSagaEasingLibrary::CatmullRomSpline(prog, 1.0, 1.5, 0.8, 0.0);
		target_yscale = FSagaEasingLibrary::CatmullRomSpline(prog, 1.0, 0.5, 0.3, 0.0);
	}
	break;

	default:
	{
		target_xscale = 0.0;
		target_yscale = 0.0;
	}
	break;
	}

	return FVector2D{ target_xscale, target_yscale };
}

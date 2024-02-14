
#include "SMAttributeProgressBar.h"
#include "Components/ProgressBar.h"

void USMAttributeProgressBar::SetProgressPercantage(float Percentage)
{
	HealthProgressBar->SetPercent(Percentage);
}

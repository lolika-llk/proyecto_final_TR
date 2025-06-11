from django.db import models

# Create your models here.
class hist(models.Model):
    temperatura=models.FloatField()
    humedad=models.IntegerField()
    presion=models.IntegerField()
    fecha_hora=models.DateTimeField(auto_now_add=True)
    class Meta:
        get_latest_by="fecha_hora"

from django.shortcuts import render
from django.http import JsonResponse
from .models import hist
import json
from django.views.decorators.csrf import csrf_exempt

# Create your views here.
def espmon(request):
    return render(request,"index.html")
def data(request):
    if request.method=='POST':
        datos=json.loads(request.body)
        t=datos.get('temperatura')
        h=datos.get('humedad')
        p=datos.get('presion')
        print(f"temp={t}\nhum={h}\n\n")
        historial=hist(temperatura=t,humedad=h, presion=p)
        historial.save()
        return JsonResponse({'status':'ok'})
    #elif request.methos=='GET':
        '''datos=hist.objects.order_by('fecha_hora')
        response={
            'fecha':[dato.fecha_hora.strftime('%Y-%m-%d %H:%M')for dato in datos],
            'temp':[dato.temperatura for dato in datos],
            'hum':[dato.humedad for dato in datos]
            }'''
        #return JsonResponse(response)
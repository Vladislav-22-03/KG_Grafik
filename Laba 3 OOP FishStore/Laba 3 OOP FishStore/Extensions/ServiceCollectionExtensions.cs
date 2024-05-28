using FishStore.Logic.Extensions;
using Laba_3_OOP_FishStore.Features.Interfaces.Managers;
using Laba_3_OOP_FishStore.Features.Managers;

namespace Laba_3_OOP_FishStore.Extensions
{
    public static class ServiceCollectionExtensions
    {
        public static void AddWebServices(this IServiceCollection services)
        {
            services.AddLogicServices();

            services.AddTransient<ICenterManager, CenterManager>();
            services.AddTransient<IUserManager, UserManager>();
        }
    }
}

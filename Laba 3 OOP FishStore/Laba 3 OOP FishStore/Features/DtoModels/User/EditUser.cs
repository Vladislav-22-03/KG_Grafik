using System.ComponentModel.DataAnnotations;

namespace Laba_3_OOP_FishStore.Features.DtoModels.User
{
    public class EditUser
    {

        public Guid? IsnNode { get; set; }


        [Required, MaxLength(255)]
        public string Name { get; set; }

        [Required, MaxLength(255)]
        public string SurName { get; set; }

        [Required, MaxLength(255)]
        public string Password { get; set; }
    }
}

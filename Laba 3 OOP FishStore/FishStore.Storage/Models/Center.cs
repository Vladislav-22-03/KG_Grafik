using Microsoft.EntityFrameworkCore;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace FishStore.Storage.Models
{
    [Index(nameof(Name))]
    public class Center
    {
        [Key, DatabaseGenerated(DatabaseGeneratedOption.None)]
        public Guid IsnNode { get; set; }

        [Required, MaxLength(20)]
        public string Code { get; set; }

        [Required, MaxLength(255)]
        public string Name { get; set; }

        //[InverseProperty(nameof(User.Center))]
        //public virtual ICollection<User> Users { get; set; }
    }
}
